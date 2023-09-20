'''
requirements:

base58==1.0.0
ecdsa==0.13
flask==2.0.2
flask-crontab==0.1.2
flask-marshmallow==0.14.0
xmltodict==0.12.0

https://flask-marshmallow.readthedocs.io/en/latest/
'''
from functools import lru_cache

import time

import os
import json
import math
import hashlib
import base58
import ecdsa
import itertools
import binascii
import pandas as pd
from collections import defaultdict
from datetime import datetime
# from flask import Flask, jsonify

# try:  # if is python3
#     from urllib.request import urlopen
# except ImportError:  # if is python2
#     from urllib2 import urlopen

seed = 1234
difficulty = 0

size_bits = 32
cripto_curve = ecdsa.SECP256k1
address_algorithm = 'sha224'

datetime_format = '%Y-%m-%d %H:%M:%S.%f'

# size_bits = 48
# cripto_curve = ecdsa.BRAINPOOLP384r1
# address_algorithm = 'sha384'

precision = 12
threshold = 10 ** -precision
threshold2 = 10 ** -7
super_supply = 100000000


def generate_private_key():
    return binascii.hexlify(os.urandom(size_bits)).decode('utf-8')


def private_key_to_public_key(private_key):
    public_key = hashlib.md5(private_key.encode()).hexdigest()
    return public_key


def public_key_to_address(public_key, unit):
    address = hashlib.md5((public_key + unit).encode()).hexdigest()
    return address


def sign_message(private_key, message):
    pos = int(len(message) / 2)
    return message[pos]


def verify_message(public_key, message, sign):
    pos = int(len(message) / 2)
    return message[pos] == sign


class Blockchain:
    def __init__(self, seed, difficulty):
        self.seed = seed
        self.difficulty = difficulty
        self.blocks = []
        self.deposits = defaultdict(float)
        self.withdraws = defaultdict(float)
        self.qty_before = defaultdict(float)
        self._null_wallet = NullWallet()
        self.foundation_wallets = {}
        self.client_wallets = {}
        self.make_genesis(seed, difficulty)
        self.cached_txtos = {}

    def __str__(self):
        return json.dumps(Blockchain.to_dict(self))

    def __iter__(self):
        return iter(self.blocks)

    @staticmethod
    def to_dict(blockchain):
        blocks = []
        for i, block in enumerate(blockchain):
            blocks.append(Block.to_dict(block))
        return blocks

    @staticmethod
    def save(blockchain, fullpath):
        blocks = Blockchain.to_dict(blockchain)
        seria = json.dumps(blocks)
        with open(fullpath, 'wb') as f:
            f.write(seria.encode('utf-8'))

    @staticmethod
    def load(fullpath):
        with open(fullpath, 'rb') as f:
            content = f.read()
        blockchain = Blockchain(seed, difficulty)
        for block_data in json.loads(content):
            block = Block.from_dict(block_data)
            # block.pow()
            # blockchain.accept_block(block)
            blockchain.accept_block(block, force=True)
        # assert(blockchain.valid())
        return blockchain

    def make_genesis(self, seed, bits):
        genesis_bus = BusBlock()
        genesis = Block(self.hash(), seed, genesis_bus, 0, difficulty=bits)
        genesis.pow()
        self.accept_block(genesis)

    def make_money(self, foundation_wallet, supply, unit):
        supply_bus = BusBlock()
        supply_bus.send(self, self._null_wallet, foundation_wallet, supply, unit,
                        description='{} FOUNDATION'.format(unit))
        block = self.make_block(supply_bus)
        block.pow()
        self.accept_block(block)

    def build_foundation_wallet(self, hash_wallet, unit, supply):
        key = hash_wallet
        if key not in self.foundation_wallets:
            wallet = self.make_foundation_wallet(hash_wallet, unit, supply)
            self.foundation_wallets[key] = wallet
        return self.foundation_wallets[key]

    def build_client_wallet(self, hash_wallet, unit):
        key = hash_wallet
        if key not in self.client_wallets:
            wallet = self.make_client_wallet(hash_wallet, unit)
            self.client_wallets[key] = wallet
        return self.client_wallets[key]

    def last(self):
        return self.blocks[-1]

    def hash(self):
        hashes = []
        for block in self.blocks:
            hashes.append(block.hash())
        return Blockchain.calculate_root_merkle_hash(hashes)

    def make_block(self, busblock):
        return Block(self.hash(), self.last().hash(), busblock, 0, difficulty=self.difficulty)

    def make_foundation_wallet(self, hash, unit, supply):
        wallet = FoundationWallet(hash, unit)
        blockchain.make_money(wallet, supply, unit)
        return wallet

    def make_client_wallet(self, hash, unit):
        wallet = HashWallet(hash, unit)
        self.init_profit(wallet, unit)
        return wallet

    def accept_block(self, block, force=False):
        if not block.solved and not force:
            raise Exception('Block is not solved')
        # register txtos in caches
        for trans in block.transactions:
            txid = trans.data.hash()
            # print(f'registering transaction: {txid}')
            for vout, txto in enumerate(trans.data._tos):
                self.cached_txtos[(txid, vout)] = trans, txto
        for trans in block.transactions:
            for txfrom in trans.data._froms:
                _, txto = self.get_txto(txfrom.txid, txfrom.vout)
                valid_sign = verify_message(txto.public_key, txto.hash(), txfrom.signature)
                if not valid_sign:
                    raise Exception('Verify lock / unlock scripts are invalid.')
        # print(f'new block: {block}')
        self.blocks.append(block)

    def get_txto(self, txid, vout):
        '''
        cached
        '''
        return self.cached_txtos[(txid, vout)]

    def is_unspent(self, transaction, txto):
        txid = transaction.data.hash()
        for block in self.blocks:
            for trans in block.transactions:
                if transaction.data.unit == trans.data.unit:
                    for txfrom in trans.data._froms:
                        _, txto_solved = self.get_txto(txfrom.txid, txfrom.vout)
                        if txfrom.txid == txid and \
                                txto_solved.public_key == txto.public_key and \
                                txto_solved.qty == txto.qty:
                            return False
        return True

    def calculate_balance(self, public_key, unit):
        balance = 0.0
        for block in self.blocks:
            for transaction in block.transactions:
                if transaction.data.unit == unit:
                    for txto in transaction.data._tos:
                        if txto.public_key == public_key:
                            if self.is_unspent(transaction, txto):
                                balance += txto.qty
        return balance

    def calculate_unspent(self, public_key, qty, unit):
        '''
        Devuelve UTXOS no gastados suficientes para realizar el pago de *qty* en *unit* a *public_key*

        Si *qty* es None, devuelve todos
        '''
        total_qty = qty
        candidates = []
        for block in blockchain.blocks:
            for transaction in block.transactions:
                if transaction.data.unit == unit:
                    for vout, txto in enumerate(transaction.data._tos):
                        if total_qty is not None and total_qty <= threshold:
                            break
                        if txto.public_key == public_key:
                            if self.is_unspent(transaction, txto):
                                if total_qty is not None:
                                    total_qty -= txto.qty
                                candidates.append((transaction.data, txto, vout))
        return candidates

    def append(self, bus):
        new_block = self.make_block(bus)
        new_block.pow()
        self.accept_block(new_block)
        if self.valid():
            return new_block
        else:
            self.blocks.pop()
            raise Exception('Error appending BusBlock.')

    @staticmethod
    def calculate_root_merkle_hash(hashes):
        while len(hashes) > 1:
            if len(hashes) % 2 != 0:
                hashes.append(hashes[-1])
            i = 0
            j = 0
            while i + 1 < len(hashes):
                hashes[j] = hashlib.md5(str(hashes[i] + hashes[i + 1]).encode('utf-8')).hexdigest()
                i += 2
                j += 1
            hashes = hashes[:int(len(hashes) / 2)]
        if len(hashes) > 0:
            return hashes[0]
        else:
            return '{:064d}'.format(0)

    def valid(self):
        # check integrity
        i = max(0, len(self.blocks) - 3)
        while i < len(self.blocks) - 1:
            if self.blocks[i].version != self.blocks[i + 1].version:
                return False
            if self.blocks[i].hash() != self.blocks[i + 1].prev_hash:
                return False
            if self.blocks[i].merkle_root == self.blocks[i + 1].merkle_root:
                return False
            if self.blocks[i].timestamp >= self.blocks[i + 1].timestamp:
                return False
            if not self.blocks[i].hash().startswith(self.blocks[i].target):
                return False
            for trans in self.blocks[i].transactions:
                if len(trans.data._froms) > 0:
                    if abs(trans.data.total_input(self, trans.data.unit) - trans.data.total_output(self,
                                                                                                   trans.data.unit)) > threshold2:
                        print('warning {} {} vs {} {}'.format(trans.data.total_input(trans.data.unit), trans.data.unit,
                                                              trans.data.total_output(trans.data.unit),
                                                              trans.data.unit))
                        return False
            i += 1
        return True

    def init_profit(self, wallet, unit):
        self.deposits[(wallet, unit)] = 0
        self.withdraws[(wallet, unit)] = 0
        self.qty_before[unit] = wallet.balance(self, unit)

    def balance_adjusted(self, wallet, unit):
        qty_after = wallet.balance(self, unit)
        profit = qty_after - self.qty_before[unit] + self.withdraws[(wallet, unit)] - self.deposits[(wallet, unit)]
        return profit

    def exchange(self, from_wallet, from_supply, baseQty, baseUnit, to_supply, to_wallet, quoteQty, quoteUnit,
                 timestamp=None, description=None, native_currency=None, native_amount=None, native_commission=None, group=None):
        bus = BusBlock()
        bus.doble_send(self, from_wallet, from_supply, baseQty, baseUnit, to_supply, to_wallet, quoteQty, quoteUnit,
                       timestamp=timestamp, description=description, native_currency=native_currency,
                       native_amount=native_amount, native_commission=native_commission, group=group)
        self.append(bus)

    def transfer(self, from_wallet, to_wallet, qty, unit, timestamp=None, description=None, native_currency=None,
                 native_amount=None, native_commission=None, group=None):
        bus = BusBlock()
        bus.send(self, from_wallet, to_wallet, qty, unit, timestamp=timestamp, description=description,
                 native_currency=native_currency, native_amount=native_amount, native_commission=native_commission, group=group)
        self.append(bus)
        self.withdraws[(from_wallet, unit)] = self.withdraws[(from_wallet, unit)] + qty
        self.deposits[(to_wallet, unit)] = self.deposits[(to_wallet, unit)] + qty

    def send(self, from_wallet, to_wallet, qty_, unit_, timestamp=None, description=None, native_currency=None,
             native_amount=None, native_commission=None, group=None):
        bus = BusBlock()
        bus.send(self, from_wallet, to_wallet, qty_, unit_, timestamp=timestamp, description=description,
                 native_currency=native_currency, native_amount=native_amount, native_commission=native_commission, group=group)
        self.append(bus)


class Block:
    def __init__(self, merkle_root, prev_hash, transactions, nonce, difficulty):
        self.version = 0x02000000
        self.prev_hash = prev_hash
        self.merkle_root = merkle_root
        self.difficulty = difficulty
        self.target = "0" * difficulty
        self.nonce = nonce
        self.timestamp = datetime.now()
        self.transactions = transactions
        self.solved = False

    def __str__(self):
        return json.dumps(Block.to_dict(self))

    @staticmethod
    def to_dict(block):
        data = {}
        data['version'] = block.version
        data['prev_hash'] = block.prev_hash
        data['merkle_root'] = block.merkle_root
        data['difficulty'] = block.difficulty
        data['target'] = block.target
        data['nonce'] = block.nonce
        data['timestamp'] = block.timestamp.strftime(datetime_format)
        transactions = []
        for trans in block.transactions:
            transactions.append(TransactionWrap.to_dict(trans))
        data['transactions'] = transactions
        return data

    @staticmethod
    def from_dict(kwargs):
        merkle_root = kwargs['merkle_root']
        prev_hash = kwargs['prev_hash']
        nonce = kwargs['nonce']
        difficulty = kwargs['difficulty']
        transactions = []
        for trans in kwargs['transactions']:
            transactions.append(TransactionWrap.from_dict(trans))
        block = Block(merkle_root, prev_hash, transactions, nonce, difficulty)
        block.timestamp = datetime.strptime(kwargs['timestamp'], datetime_format)
        block.version = kwargs['version']
        block.target = kwargs['target']
        return block

    def hash(self):
        return hashlib.md5(self.__str__().encode()).hexdigest()

    def check_nonce(self, nonce, bits):
        self.nonce = nonce
        guess_hash = self.hash()
        return guess_hash[:bits] == self.target

    def pow(self):
        for nonce in itertools.count(start=self.nonce):  # inifinite generator by brute force
            if self.check_nonce(nonce, self.difficulty):
                self.solved = True
                return nonce


class TxFrom:
    def __init__(self, txid, vout, signature):
        self.txid = txid
        self.vout = vout
        self.signature = signature

    @staticmethod
    def to_dict(txfrom):
        data = {}
        data['txid'] = txfrom.txid
        data['vout'] = txfrom.vout
        data['signature'] = txfrom.signature
        return data

    @staticmethod
    def from_dict(kwargs):
        txid = kwargs['txid']
        vout = kwargs['vout']
        signature = kwargs['signature']
        txfrom = TxFrom(txid, vout, signature)
        return txfrom

    def __str__(self):
        return json.dumps(TxFrom.to_dict(self))

    def hash(self):
        return hashlib.md5(self.__str__().encode()).hexdigest()


class TxTo:
    def __init__(self, qty, public_key):
        self.qty = round(qty, precision)
        self.public_key = public_key

    @staticmethod
    def to_dict(txto):
        data = {}
        data['qty'] = txto.qty
        data['public_key'] = txto.public_key
        return data

    @staticmethod
    def from_dict(kwargs):
        qty = kwargs['qty']
        public_key = kwargs['public_key']
        txto = TxTo(qty, public_key)
        return txto

    def __str__(self):
        return json.dumps(TxTo.to_dict(self))

    def hash(self):
        return hashlib.md5(self.__str__().encode()).hexdigest()


class Transaction:
    def __init__(self, qty, unit, timestamp=None, description=None, native_currency=None, native_amount=None, native_commission=None, group=None):
        self.qty = qty
        self.unit = unit
        if timestamp is None:
            self.timestamp = datetime.now()
        else:
            self.timestamp = timestamp
        if description is None:
            self.description = ""
        else:
            self.description = description
        self.native_currency = native_currency
        self.native_amount = native_amount
        self.native_commission = native_commission
        self.group = group
        self._froms = []
        self._tos = []

    @staticmethod
    def to_dict(trans):
        data = {}
        data['txid'] = trans.hash()
        data['qty'] = trans.qty
        data['unit'] = trans.unit
        data['timestamp'] = trans.timestamp.strftime(datetime_format)
        data['description'] = trans.description
        data['native_currency'] = trans.native_currency
        data['native_amount'] = trans.native_amount
        data['native_commission'] = trans.native_commission
        data['group'] = trans.group
        froms = []
        for txfro in trans._froms:
            froms.append(TxFrom.to_dict(txfro))
        data['froms'] = froms
        tos = []
        for txto in trans._tos:
            tos.append(TxTo.to_dict(txto))
        data['tos'] = tos

        data2_hash = Transaction.from_dict(data)
        data_hash = trans
        data2_hash = data2_hash.hash()
        data_hash = data_hash.hash()
        assert(data2_hash == data_hash)

        return data

    @staticmethod
    def from_dict(kwargs):
        qty = kwargs['qty']
        unit = kwargs['unit']
        timestamp = datetime.strptime(kwargs['timestamp'], datetime_format)
        description = kwargs['description']
        native_currency = kwargs['native_currency']
        native_amount = kwargs['native_amount']
        native_commission = kwargs['native_commission']
        group = kwargs['group']
        trans = Transaction(qty, unit, timestamp=timestamp, description=description, native_currency=native_currency, native_amount=native_amount, native_commission=native_commission, group=group)
        for txfrom in kwargs['froms']:
            trans._froms.append(TxFrom.from_dict(txfrom))
        for txto in kwargs['tos']:
            trans._tos.append(TxTo.from_dict(txto))
        assert(trans.hash() == kwargs['txid'])
        return trans

    def __str__(self):
        return json.dumps(Transaction.to_dict(self))

    def add_from(self, txid, vout, signature):
        self._froms.append(TxFrom(txid, vout, signature))

    def add_to(self, qty, public_key):
        self._tos.append(TxTo(qty, public_key))

    def total_input(self, blockchain, unit):
        qty = 0.0
        for txfrom in self._froms:
            _, txto_solved = blockchain.get_txto(txfrom.txid, txfrom.vout)
            if self.unit == unit:
                qty += txto_solved.qty
        return qty

    def total_output(self, blockchain, unit):
        qty = 0.0
        for txto in self._tos:
            if self.unit == unit:
                qty += txto.qty
        return qty

    def hash(self):
        message = str(self.timestamp) + '\n'
        for txfro in self._froms:
            message += '{}\n'.format(txfro.hash())
        for txto in self._tos:
            message += '{}\n'.format(txto.hash())
        return hashlib.md5(message.encode()).hexdigest()


class TransactionWrap:
    def __init__(self, data, sign, public):
        self.data = data
        self.sign = sign
        self.public = public

    @staticmethod
    def to_dict(trans):
        data = {}
        data['data'] = Transaction.to_dict(trans.data)
        data['sign'] = trans.sign
        data['public'] = trans.public
        return data

    @staticmethod
    def from_dict(kwargs):
        data = Transaction.from_dict(kwargs['data'])
        sign = kwargs['sign']
        public = kwargs['public']
        trans = TransactionWrap(data, sign, public)
        return trans

    def __str__(self):
        return json.dumps(TransactionWrap.to_dict(self))


class BusBlock:
    def __init__(self):
        self.transactions = []

    def __iter__(self):
        return iter(self.transactions)

    def __len__(self):
        return len(self.transactions)

    def __getitem__(self, item):
        return self.transactions[item]

    def doble_send(self, blockchain, from_wallet, from_supply, baseQty, baseUnit, to_supply, to_wallet, quoteQty,
                   quoteUnit, timestamp=None, description=None, native_currency=None, native_amount=None, native_commission=None, group=None):
        self.send(blockchain, from_wallet, from_supply, baseQty, baseUnit, timestamp=timestamp,
                  description='1/2 ' + description, native_currency=native_currency, native_amount=native_amount, native_commission=native_commission, group=group)
        self.send(blockchain, to_supply, to_wallet, quoteQty, quoteUnit, timestamp=timestamp,
                  description='2/2 ' + description, native_currency=native_currency, native_amount=native_amount, native_commission=native_commission, group=group)

    def send(self, blockchain, from_wallet, to_wallet, qty_, unit_, timestamp=None, description=None,
             native_currency=None, native_amount=None, native_commission=None, group=None):

        transaction = Transaction(qty_, unit_, timestamp=timestamp, description=description,
                                  native_currency=native_currency, native_amount=native_amount, native_commission=native_commission, group=group)
        if from_wallet != blockchain._null_wallet:

            txtos = blockchain.calculate_unspent(from_wallet.public, qty_, unit_)

            for trans, txto, vout in txtos:
                serialized_txto = txto.hash()
                signature = sign_message(from_wallet.private, serialized_txto)
                transaction.add_from(trans.hash(), vout, signature)

            spent_qty = qty_
            group_txtos = defaultdict(float)
            for _, txto, _ in txtos:
                # el monto es mayor que la cantidad necesaria, devuelveme el cambio
                if txto.qty > spent_qty:
                    group_txtos[to_wallet.public] = group_txtos[to_wallet.public] + spent_qty
                    change = txto.qty - spent_qty
                    if change > threshold:
                        group_txtos[from_wallet.public] = group_txtos[from_wallet.public] + change
                    spent_qty -= spent_qty
                else:
                    group_txtos[to_wallet.public] = group_txtos[to_wallet.public] + txto.qty
                    spent_qty -= txto.qty
                if abs(spent_qty) <= threshold:
                    break

            '''
            Agrupar todos los TxTo que van a la misma direcciÃ³n
            minimizar el nÃºmero de salidas para ahorrar bytes
            '''
            for k, v in group_txtos.items():
                # TODO add cantidad en euros y comision en euros prorateada
                transaction.add_to(v, k)

            if abs(spent_qty) > threshold:
                print("error {}: {} {} {} {}".format(spent_qty, from_wallet, to_wallet, qty_, unit_))
        else:
            # txto foundation
            transaction.add_to(qty_, to_wallet.public)
        message = str(transaction)
        sign = sign_message(from_wallet.private, message)
        valid_sign = verify_message(from_wallet.public, message, sign)
        if not valid_sign:
            raise Exception('Error generating sign.')
        transaction_wrap = TransactionWrap(transaction, sign, from_wallet.public)
        self.transactions.append(transaction_wrap)
        return transaction


class PublicWallet:
    def __init__(self, unit, private_key, suffix=None):
        self.public = private_key_to_public_key(private_key)
        if suffix is not None:
            unit = unit + suffix
        self.address = public_key_to_address(self.public, unit)

    def __str__(self):
        return self.address

    def __eq__(self, other):
        if isinstance(other, str):
            return self.address == other
        else:
            return self.address == other.address

    def __hash__(self):
        return hash(self.address)

    def endpoint(self):
        return self.address

    def balance(self, blockchain, unit, format=False):
        '''
        txto = income
        txfrom = expenses
        balance = sum(txto) - sum(txfrom)
        '''
        if format:
            return '{} {}'.format(blockchain.calculate_balance(self.public, unit), unit)
        else:
            return blockchain.calculate_balance(self.public, unit)


class PrivateWallet(PublicWallet):
    def __init__(self, unit, private_key=None, suffix=None):
        if private_key is None:
            self.private = generate_private_key()
        else:
            self.private = binascii.hexlify(private_key.encode('utf-8')).decode('utf-8')
        super().__init__(unit, self.private, suffix=suffix)


class NullWallet(PublicWallet):
    def __init__(self):
        self.unit = 'NULL'
        self.private = binascii.hexlify(('0' * size_bits).encode('utf-8')).decode('utf-8')
        super().__init__(self.unit, self.private)


class HashWallet(PrivateWallet):
    def __init__(self, name, unit):
        self.name = name
        self.unit = unit
        private = hashlib.md5(name.encode()).hexdigest()[:size_bits]
        super().__init__(unit, private)


class FoundationWallet(PrivateWallet):
    def __init__(self, name, unit):
        self.name = name
        self.unit = unit
        private = hashlib.md5(name.encode()).hexdigest()[:size_bits]
        super().__init__(self.unit, private, suffix="_FOUNDATION")


if __name__ == '__main__':

    begin = time.time()

    dataset_csv = os.path.join (os.path.dirname ( os.path.abspath ( __file__ ) ), "ricardo/binance.csv")
    df1 = pd.read_csv(dataset_csv)
    df1['Group'] = 'binance'

    dataset_csv = os.path.join (os.path.dirname ( os.path.abspath ( __file__ ) ), "ricardo/coinbasepro.csv")
    df2 = pd.read_csv(dataset_csv)
    df2['Group'] = 'coinbasepro'
    df2['Native Commission Amount'] = 0.0

    dataset_csv = os.path.join (os.path.dirname ( os.path.abspath ( __file__ ) ), "ricardo/crypto_transactions_record_20211213_195307.csv")
    df3 = pd.read_csv(dataset_csv)
    df3['Group'] = 'cripto.com'
    df3['Native Commission Amount'] = 0.0

    dataset_csv = os.path.join (os.path.dirname ( os.path.abspath ( __file__ ) ), "ricardo/coinbase.csv")
    df4 = pd.read_csv(dataset_csv)
    df4['Group'] = 'coinbase'

    dataset_csv = os.path.join (os.path.dirname ( os.path.abspath ( __file__ ) ), "ricardo/bittrex.csv")
    df5 = pd.read_csv(dataset_csv)
    df5['Group'] = 'bittrex'
    df5['Native Commission Amount'] = 0.0


    df1.drop_duplicates(inplace=True)
    df2.drop_duplicates(inplace=True)
    df3.drop_duplicates(inplace=True)
    df4.drop_duplicates(inplace=True)
    df5.drop_duplicates(inplace=True)

    df = pd.concat([df1, df2, df3, df4, df5], join='outer')


    # dataset_csv = os.path.join (os.path.dirname ( os.path.abspath ( __file__ ) ), "andoitz/binance.csv")
    # df = pd.read_csv(dataset_csv)
    # df['Group'] = 'binance'
    # df.drop_duplicates(inplace=True)



    df.index = pd.to_datetime(df['Timestamp (UTC)'])
    df.drop('Timestamp (UTC)', axis=1, inplace=True)
    df = df.sort_index()
    df.to_csv('blockchain.csv')

    blockchain = Blockchain(seed, difficulty)

    currencies = []
    for index, row in df.iterrows():
        timestamp = index.to_pydatetime()
        currency = row['Currency']
        amount = row['Amount']
        native_currency = row['Native Currency']
        native_amount = row['Native Amount']
        to_currency = row['To Currency']
        to_amount = row['To Amount']
        kind = row['Transaction Kind']
        native_commission = row['Native Commission Amount']
        group = row['Group']

        if currency not in currencies:
            if isinstance(currency, str) or not math.isnan(currency):
                currencies.append(currency)
        if to_currency not in currencies:
            if isinstance(currency, str) or not math.isnan(to_currency):
                currencies.append(to_currency)

        if (kind == 'crypto_withdrawal') or (kind == 'crypto_wallet_swap_debited') or (kind == 'card_top_up'):

            # from_foundation_wallet = blockchain.build_foundation_wallet('Central deposit {}'.format(currency), currency,
            #                                                             super_supply)
            # from_personal_wallet = blockchain.build_client_wallet('Personal {}'.format(currency), currency)
            #
            # blockchain.transfer(from_personal_wallet, from_foundation_wallet, -amount, currency,
            #                     timestamp=timestamp,
            #                     description='withdraw or pay {} {}'.format(-amount, currency),
            #                     native_currency=native_currency,
            #                     native_amount=native_amount,
            #                     native_commission=native_commission,
            #                     group=group)

            pass

        elif (kind == 'crypto_deposit') or (kind == 'crypto_wallet_swap_credited'):

            # from_foundation_wallet = blockchain.build_foundation_wallet('Central deposit {}'.format(currency), currency,
            #                                                             super_supply)
            # from_personal_wallet = blockchain.build_client_wallet('Personal {}'.format(currency), currency)
            #
            # blockchain.transfer(from_foundation_wallet, from_personal_wallet, amount, currency,
            #                     timestamp=timestamp,
            #                     description='deposit or reward {} {}'.format(amount, currency),
            #                     native_currency=native_currency,
            #                     native_amount=native_amount,
            #                     native_commission=native_commission,
            #                     group=group)
            pass

        elif (kind == 'dynamic_coin_swap_debited') or (kind == 'dynamic_coin_swap_credited') or (
                kind == 'dynamic_coin_swap_bonus_exchange_deposit') or (kind == 'lockup_lock'):
            pass

        elif (kind == 'reimbursement'):

            # from_foundation_wallet = blockchain.build_foundation_wallet('Central deposit {}'.format(currency), currency,
            #                                                             super_supply)
            # from_personal_wallet = blockchain.build_client_wallet('Personal {}'.format(currency), currency)
            #
            # blockchain.send(from_foundation_wallet, from_personal_wallet, amount, currency,
            #                 timestamp=timestamp,
            #                 description='send {} {}'.format(amount, currency),
            #                 native_currency=native_currency,
            #                 native_amount=native_amount,
            #                 native_commission=native_commission,
            #                 group=group)
            pass

        elif (kind == 'crypto_exchange') or (kind == 'crypto_viban_exchange'):

            from_foundation_wallet = blockchain.build_foundation_wallet('Central deposit {}'.format(currency), currency,
                                                                        super_supply)
            from_personal_wallet = blockchain.build_client_wallet('Personal {}'.format(currency), currency)

            to_foundation_wallet = blockchain.build_foundation_wallet('Central deposit {}'.format(to_currency), to_currency,
                                                                      super_supply)
            to_personal_wallet = blockchain.build_client_wallet('Personal {}'.format(to_currency), to_currency)

            blockchain.exchange(from_personal_wallet, from_foundation_wallet, -amount, currency,
                                to_foundation_wallet, to_personal_wallet, to_amount, to_currency,
                                timestamp=timestamp,
                                description='exchange B {} {} -> {} {}'.format(-amount, currency, to_amount, to_currency),
                                native_currency=native_currency,
                                native_amount=native_amount,
                                native_commission=native_commission,
                                group=group)

        elif (kind == 'viban_purchase'):

            from_foundation_wallet = blockchain.build_foundation_wallet('Central deposit {}'.format(currency), currency,
                                                                        super_supply)
            from_personal_wallet = blockchain.build_client_wallet('Personal {}'.format(currency), currency)

            to_foundation_wallet = blockchain.build_foundation_wallet('Central deposit {}'.format(to_currency), to_currency,
                                                                      super_supply)
            to_personal_wallet = blockchain.build_client_wallet('Personal {}'.format(to_currency), to_currency)

            if currency == native_currency:

                blockchain.transfer(from_foundation_wallet, from_personal_wallet, -amount, currency, timestamp=timestamp,
                                    description='recharge {} {}'.format(-amount, currency),
                                    native_currency=native_currency,
                                    native_amount=native_amount,
                                    native_commission=native_commission,
                                    group=group)

            blockchain.exchange(from_personal_wallet, from_foundation_wallet, -amount, currency,
                                to_foundation_wallet, to_personal_wallet, to_amount, to_currency,
                                timestamp=timestamp,
                                description='exchange A {} {} -> {} {}'.format(-amount, currency, to_amount, to_currency),
                                native_currency=native_currency,
                                native_amount=native_amount,
                                native_commission=native_commission,
                                group=group)

        else:
            print(kind + ' --')
            print(row)

    if True:
        '''
        consejos cointracking:
        https://bitcoinnovatos.com/cointracking/cointracking-guia-tutorial-completa-declaracion-renta-t47.html
        '''
        # en euros
        current_prices = {
            'ETH': 3368.49817155,
            'CRO': 0.48623961,
            'VET': 0.08355011,
            'XRP': 0.73116918,
            'LINK': 23.42694297,
            'XTZ': 4.56812277,
            'LTC': 131.66515817,
            'IOTX': 0.05422952,
            'ATOM': 35.42029296,
            'GLM': 0.42661021,
            'SOL': 148.92343406,
            'ARDR': 0.22439384,
            'ADA': 1.17072875,
            'RVN': 0.11730183,
            'FTM': 2.72115944,
            'BAT': 1.16600343,
            'BTC': 41022.64,
            'ELON': 0.00000135,
            'DOT': 26.65410210,
            'ONE': 0.28542549,
            'CSPR': 0.11859538,
            'SHIB': 0.00002888,
            'HBAR': 0.28636641,
            'IRIS': 0.08240904,
            'FIL': 32.75690724,
            'JUV': 7.59989499,
            'PSG': 13.42166136,
            'CAKE': 10.58414258,
            'USDT': 0.88349627,
            'WTC': 0.85095721,
            'ZIL': 0.06405662,
            'ANKR': 0.10066231,
            'MCO': 14.13302729,
            'BAND': 5.45409010,
            'DOGE': 0.15011814,
            'IOTA': 0.985261,
        }
        print('-- P&L --')
        dataset_final = []
        dataset_sells = []
        dataset_buys = []
        total_buy_value = defaultdict(float)
        total_sell_value = defaultdict(float)
        for block in blockchain:
            for trans in block.transactions:

                trans_value = trans.data.native_amount
                if trans_value is None or trans_value < 0:
                    continue

                currency = trans.data.unit
                if currency == 'EUR':
                    continue

                txid = trans.data.hash()
                group = trans.data.group

                personal_wallet = blockchain.build_client_wallet('Personal {}'.format(currency), currency)
                central_wallet = blockchain.build_foundation_wallet('Central deposit {}'.format(currency), currency,
                                                                    super_supply)

                is_buy_trade = (central_wallet == trans.public)
                is_sell_trade = (personal_wallet.public == trans.public)


                # si la transacciÃ³n procede de su cuenta personal ..
                if not is_sell_trade:
                    # unspent_total = 0.0
                    # spent_total = 0.0
                    # for txto in trans.data._tos:
                    #     if txto.public_key == trans.public:
                    #         if blockchain.is_unspent(trans, txto):
                    #             unspent_total += txto.qty
                    #         else:
                    #             spent_total += txto.qty
                    #
                    # print('spent_total = {}'.format(spent_total))
                    # print('unspent_total = {}'.format(unspent_total))
                    #
                    # # Es closed, si algun TxTo esta gastado
                    # cost_per_unit = trans_value / trans.data.qty
                    # value = trans.data.qty * current_prices[currency]
                    # cost = trans.data.native_amount
                    # profit = value - cost
                    # performance = 100.0 * ((current_prices[currency] / cost_per_unit) - 1.0)
                    # dataset_buys.append(('Buy', trans.data.qty, currency, cost_per_unit, current_prices[currency], performance,
                    #                      cost, value, profit, trans.data.timestamp, txid))
                    # print(dataset_buys[-1])
                    continue

                print('txid venta = {}'.format(trans.data.hash()))
                print('currency: {}'.format(currency))
                print('cantidad venta eur = {}'.format(trans.data.native_amount))
                print('cantidad venta base = {}'.format(trans.data.qty))
                print('precio venta = {}'.format(trans.data.native_amount / trans.data.qty))
                print('  ---')

                buy_date = None
                sell_date = trans.data.timestamp
                commission_sell = trans.data.native_commission

                # buscar utxos no gastados (operaciones no cerradas)

                # open_amount = 0.0
                # for transaction, txto, vout in blockchain.calculate_unspent(trans.public, None, currency):
                #     open_amount += txto.qty

                # carry = 0.0
                # for txto in trans.data._tos:
                #     if txto.public_key == trans.public:
                #         if blockchain.is_unspent(trans, txto):
                #             carry += txto.qty

                # buscar utxos gastados (operaciones cerradas)
                first = True
                buy_value = 0.0
                buy_qty = 0.0
                qty = trans.data.qty

                commission_buy = 0.0

                for txfrom in trans.data._froms:
                    txto_trans, txto = blockchain.get_txto(txfrom.txid, txfrom.vout)

                    if txto_trans.data.native_amount is None:
                        raise Exception('error, no native_amount in {}'.format(txto_trans))

                    if txto.qty < qty:
                        # gasta todo
                        qty -= txto.qty
                        spent_total = txto.qty
                    else:
                        # gasto parcial
                        spent_total = qty
                    spent_porcentage = spent_total / txto_trans.data.qty

                    if ( spent_porcentage <= 0 ):
                        raise Exception("error spent_porcentage")

                    if first:
                        buy_date = txto_trans.data.timestamp
                        first = False

                    value = spent_porcentage * txto_trans.data.native_amount
                    commission = spent_porcentage * txto_trans.data.native_commission
                    amount = spent_total
                    price = value / amount

                    # precio en euros de la compra
                    print('    txid compra = {}'.format(txto_trans.data.hash()))
                    print('    cantidad compra eur = {}'.format(txto_trans.data.native_amount))
                    print('    cantidad compra eur prorateada = {}'.format(value))
                    print('    cantidad compra base = {}'.format(txto_trans.data.qty))
                    print('    cantidad compra base prorateada = {}'.format(spent_total))
                    print('    precio compra = {}'.format(price))
                    print('    commission compra = {}'.format(commission))
                    print('       ------')

                    buy_value += value
                    buy_qty += amount
                    commission_buy += commission

                if (abs(buy_qty - trans.data.qty) > threshold):
                    raise Exception("error in sell with diff: {}".format(buy_qty - trans.data.qty))

                buy_price = buy_value / trans.data.qty
                sell_price = trans_value / trans.data.qty
                performance = 100.0 * ((sell_price / buy_price) - 1.0)

                adquisition = buy_value + commission_buy
                transmision = trans_value - commission_sell
                gross_profit = transmision - adquisition
                net_profit = trans_value - buy_value

                row = (
                    trans.data.qty, currency,
                    buy_price, buy_value, commission_buy, adquisition,
                    sell_price, trans_value, commission_sell, transmision,
                    performance,
                    gross_profit, net_profit,
                    buy_date, sell_date, group, txid
                )
                dataset_sells.append(row)

                total_buy_value[trans.data.unit] = total_buy_value[trans.data.unit] + buy_value
                total_sell_value[trans.data.unit] = total_sell_value[trans.data.unit] + trans_value

                assert (buy_date is not None)
                print(' Detalle Fecha compra / Fecha venta: {} / {}'.format(buy_date, sell_date))
                print(' Detalle value buy: {}'.format(buy_value))
                print(' Detalle value sell: {}'.format(trans_value))
                print(' Detalle commission buy: {}'.format(commission_buy))
                print(' Detalle commission sell: {}'.format(commission_sell))
                print(' Detalle net profit: {}'.format(net_profit))
                print(' Detalle gross profit: {}'.format(gross_profit))
                print(' ---')


        df = pd.DataFrame(dataset_buys,
                          columns=['Kind', 'Amount', 'Currency', 'Cost per unit', 'Price per unit', 'Performance', 'Cost',
                                   'Current Value', 'Profit', 'Buy Date', 'TxId'])
        print(df)
        print('TOTAL open net profit {}'.format(df['Profit'].sum()))
        df.to_csv('buys.csv')
        df = pd.DataFrame(dataset_sells,
                          columns=['Amount', 'Currency',
                                   'Cost per unit', 'Buy Value', 'Comm. Buy', 'Adquisition',
                                   'Price Sold per unit', 'Sold Value', 'Comm. Sell', 'Transmision',
                                   'Performance',
                                   'Gross Profit', 'Net Profit',
                                   'Buy Date', 'Sell Date', 'Group', 'TxId'])
        print(df)
        print('TOTAL close net profit {}'.format(df['Net Profit'].sum()))
        df.to_csv('sells.csv')



    print('-- balances --')
    for currency in currencies:
        if isinstance(currency, str):
            wallet = blockchain.build_client_wallet('Personal {}'.format(currency), currency)
            print('{} - balance: {}'.format(currency, wallet.balance(blockchain, currency, True)))

    # Blockchain.save(blockchain, 'blockchain.json')

    elapsed = time.time() - begin
    print(f'elapsed: {elapsed}secs.')
