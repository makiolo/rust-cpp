import json
import ctypes
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from pprint import pprint
import excelbind


def dumps_rec(serie, depth=0, recursive=False):

    if isinstance(serie, Serie):

        if serie.type == map_serie_serie_type or serie.type == map_string_serie_type:

            if serie.type == map_serie_serie_type:
                serie_map = serie.get_map_serie_serie()
            elif serie.type == map_string_serie_type:
                serie_map = serie.get_map_string_serie()
            else:
                raise Exception('Invalid type: {}'.format(serie.type))

            if recursive:
                new_map = {}
                for k, v in serie_map.items():
                    key_map = dumps_rec(k, depth+1, recursive=recursive)
                    value_map = dumps_rec(v, depth+1, recursive=recursive)
                    new_map[key_map] = value_map
                return new_map
            else:
                # return serie_map.asdict()
                return serie_map

        elif serie.type == set_serie_type:

            return serie.get_set_serie()

        elif    serie.type == basic_numpy_type or \
                serie.type == string_type or \
                serie.type == basic_none_type:

            if serie.type == basic_numpy_type:
                basic = serie.get_basic_numpy()
                if len(basic) == 1:
                    # TODO: convert List[1] to double
                    basic = basic[0]
            elif serie.type == string_type:
                basic = serie.get_basic_string()
            elif serie.type == basic_none_type:
                basic = None
            else:
                raise Exception('Invalid type: {}'.format(serie.type))

            return basic

        elif serie.type == calculation_type:

            return dumps_rec(serie.get_calc(), depth+1, recursive=recursive)

        else:
            if serie.type == vector_none_type:
                listdata = [None] * len(serie.get_vector_none())
            elif serie.type == vector_string_type:
                listdata = serie.get_vector_string()
            elif serie.type == vector_serie_type:
                listdata = serie.get_vector_serie()
            elif serie.type == vector_map_serie_serie_type:
                listdata = serie.get_vector_map_serie_serie()
            elif serie.type == vector_map_string_serie_type:
                listdata = serie.get_vector_map_string_serie()
            elif serie.type == vector_set_serie_type:
                listdata = serie.get_vector_set_serie()
            else:
                raise Exception('Invalid type: {}'.format(serie.type))

            if recursive:
                list_str = []
                for k in listdata:
                    list_str.append(dumps_rec(k, depth+1, recursive=recursive))
                listdata = list_str

            if len(listdata) == 1:
                # List[1] to double
                # TODO: use special type
                return listdata[0]
            else:
                return listdata

    else:
        # python type
        return serie


def dumps(serie, recursive=False):
    '''
    Print IL with your typing
    '''
    return dumps_rec(serie, recursive=recursive)


def loads(serie):
    '''
    Convert Python Structure to IL
    '''
    if isinstance(serie, dict):
        try:
            new_map = {}
            for k, v in serie.items():
                k_serie = loads(k)
                v_serie = loads(v)
                if not isinstance(v_serie, Serie):
                    v_serie = Serie(v_serie)
                new_map[k_serie] = v_serie
            return Serie(new_map)
        except (ValueError, TypeError):
            new_map = {}
            for k, v in serie.items():
                k_serie = loads(k)
                if not isinstance(k_serie, Serie):
                    k_serie = Serie(k_serie)
                v_serie = loads(v)
                if not isinstance(v_serie, Serie):
                    v_serie = Serie(v_serie)
                new_map[k_serie] = v_serie
            return Serie(new_map)
    elif isinstance(serie, list) or isinstance(serie, tuple):
        try:
            l = []  # list with same basic type
            for k in serie:
                l.append(loads(k))
            return Serie(l)
        except (ValueError, TypeError):
            l = []  # list multiples types differents
            for k in serie:
                elem = loads(k)
                if not isinstance(elem, Serie):
                    l.append(Serie(elem))
                else:
                    l.append(elem)
            return l
    elif isinstance(serie, set):
        try:
            l = set()  # list with same basic type
            for k in serie:
                l.add(loads(k))
            return Serie(l)
        except (ValueError, TypeError):
            l = set()  # list multiples types differents
            for k in serie:
                elem = loads(k)
                if not isinstance(elem, Serie):
                    l.add(Serie(elem))
                else:
                    l.add(elem)
            return l
    else:
        if serie is None:
            return NoneType()
        else:
            if type(serie) == unicode:
                return str(serie)
            else:
                return serie



def read_json(filename):
    with open(filename, 'r') as f:
        obj_python = json.load(f)
    return loads(obj_python)


def write_json(serie, filename, indent=False):
    obj_python = dumps(serie, recursive=True)
    with open(filename, 'w') as f:
        json.dump(obj_python, f, indent=indent)
    return obj_python



def read_il(filename):
    with open(filename, 'r') as f:
        obj_python = eval(f.read())
    return loads(obj_python)


def write_il(serie, filename):
    obj_python = dumps(serie, recursive=True)
    with open(filename, 'w') as f:
        pprint(obj_python, f)


### add methods to Serie

def Serie_get(self):
    return dumps(self)

def Serie_set(self, data):
    if not isinstance(data, Serie):
        data = Serie(data)
    python_obj = dumps(data)
    if  data.type != calculation_type and data.type != self.type or \
            data.type == calculation_type and data.get_calc().type != self.type:
        raise TypeError("Invalid type {}, expected type: {}.".format(type(python_obj), self.type))
    if self.type == basic_numpy_type:
        self.set_basic_numpy(python_obj)
    elif self.type == string_type:
        self.set_basic_string(python_obj)
    elif self.type == basic_none_type:
        self.set_basic_none()
    elif self.type == vector_string_type:
        self.set_vector_string(python_obj)
    else:
        raise Exception("Not Implemmented type: {}".format(self.type))

def Serie___getitem__(self, key):
    return dumps(self).__getitem__(key)


def Serie___iter__(self):
    return dumps(self).__iter__()


def Serie_iteritems(self):
    return dumps(self).iteritems()


def Serie__repr__(self):
    return _red_pandas.Serie___str__(self)


def Serie_to_numpy(self):
    return np.array(self.to_vector())


def Serie_plot(self):
    plt.plot(range(self.size()), self.to_numpy())


def plot_transpose(simulation):
    simulation = map(lambda e: e.to_numpy(), simulation)
    plt.plot(range(len(simulation)), simulation)


def show():
    plt.show()


Serie.get = Serie_get
Serie.set = Serie_set
Serie.iteritems = Serie_iteritems
Serie.__getitem__ = Serie___getitem__
Serie.__iter__ = Serie___iter__
Serie.__repr__ = Serie__repr__
Serie.to_numpy = Serie_to_numpy
Serie.plot = Serie_plot

# a ctypes callback prototype
py_callback_type = ctypes.CFUNCTYPE(None, ctypes.c_int, ctypes.c_char_p)

def use_callback(py_callback):

    # wrap the python callback with a ctypes function pointer
    f = py_callback_type(py_callback)

    # get the function pointer of the ctypes wrapper by casting it to void* and taking its value
    f_ptr = ctypes.cast(f, ctypes.c_void_p).value

    _red_pandas.use_callback(f_ptr)



@excelbind.function
def d1_(S:list, K:list, r:list, q:list, T:list, Vol:list) -> list:
    return _red_pandas.d1(array(S), array(K), array(r), array(q), array(T), array(Vol)).to_vector()

@excelbind.function
def d2_(S:list, K:list, r:list, q:list, T:list, Vol:list) -> list:
    return _red_pandas.d2(array(S), array(K), array(r), array(q), array(T), array(Vol)).to_vector()

@excelbind.function
def CallPrice_(S:list, K:list, r:list, q:list, T:list, Vol:list, d_uno:list, d_dos:list) -> list:
    return _red_pandas.CallPrice(array(S), array(K), array(r), array(q), array(T), array(Vol), array(d_uno), array(d_dos)).to_vector()

@excelbind.function
def PutPrice_(S:list, K:list, r:list, q:list, T:list, Vol:list, d_uno:list, d_dos:list) -> list:
    return _red_pandas.PutPrice(array(S), array(K), array(r), array(q), array(T), array(Vol), array(d_uno), array(d_dos)).to_vector()

@excelbind.function
def CallGreeksDelta_(S:list, K:list, r:list, q:list, T:list, Vol:list, d_uno:list, d_dos:list) -> list:
    return _red_pandas.CallGreeksDelta(array(S), array(K), array(r), array(q), array(T), array(Vol), array(d_uno), array(d_dos)).to_vector()

@excelbind.function
def CallGreeksGamma_(S:list, K:list, r:list, q:list, T:list, Vol:list, d_uno:list, d_dos:list) -> list:
    return _red_pandas.CallGreeksGamma(array(S), array(K), array(r), array(q), array(T), array(Vol), array(d_uno), array(d_dos)).to_vector()

@excelbind.function
def CallGreeksTheta_(S:list, K:list, r:list, q:list, T:list, Vol:list, d_uno:list, d_dos:list) -> list:
    return _red_pandas.CallGreeksTheta(array(S), array(K), array(r), array(q), array(T), array(Vol), array(d_uno), array(d_dos)).to_vector()

@excelbind.function
def CallGreeksVega_(S:list, K:list, r:list, q:list, T:list, Vol:list, d_uno:list, d_dos:list) -> list:
    return _red_pandas.CallGreeksVega(array(S), array(K), array(r), array(q), array(T), array(Vol), array(d_uno), array(d_dos)).to_vector()

@excelbind.function
def CallGreeksRho_(S:list, K:list, r:list, q:list, T:list, Vol:list, d_uno:list, d_dos:list) -> list:
    return _red_pandas.CallGreeksRho(array(S), array(K), array(r), array(q), array(T), array(Vol), array(d_uno), array(d_dos)).to_vector()

@excelbind.function
def PutGreeksDelta_(S:list, K:list, r:list, q:list, T:list, Vol:list, d_uno:list, d_dos:list) -> list:
    return _red_pandas.PutGreeksDelta(array(S), array(K), array(r), array(q), array(T), array(Vol), array(d_uno), array(d_dos)).to_vector()

@excelbind.function
def PutGreeksGamma_(S:list, K:list, r:list, q:list, T:list, Vol:list, d_uno:list, d_dos:list) -> list:
    return _red_pandas.PutGreeksGamma(array(S), array(K), array(r), array(q), array(T), array(Vol), array(d_uno), array(d_dos)).to_vector()

@excelbind.function
def PutGreeksTheta_(S:list, K:list, r:list, q:list, T:list, Vol:list, d_uno:list, d_dos:list) -> list:
    return _red_pandas.PutGreeksTheta(array(S), array(K), array(r), array(q), array(T), array(Vol), array(d_uno), array(d_dos)).to_vector()

@excelbind.function
def PutGreeksVega_(S:list, K:list, r:list, q:list, T:list, Vol:list, d_uno:list, d_dos:list) -> list:
    return _red_pandas.PutGreeksVega(array(S), array(K), array(r), array(q), array(T), array(Vol), array(d_uno), array(d_dos)).to_vector()

@excelbind.function
def PutGreeksRho_(S:list, K:list, r:list, q:list, T:list, Vol:list, d_uno:list, d_dos:list) -> list:
    return _red_pandas.PutGreeksRho(array(S), array(K), array(r), array(q), array(T), array(Vol), array(d_uno), array(d_dos)).to_vector()

@excelbind.function
def ImpliedVolatility_(S:list, K:list, r:list, q:list, T:list, Vol_guess:list, market_price:list, flag:str, tol:float=0.0001, lr:float=0.33) -> list:
    return _red_pandas.ImpliedVolatility(array(S), array(K), array(r), array(q), array(T), array(Vol_guess), array(market_price), flag, tol, lr, 100).to_vector()

@excelbind.function
def CallImpliedvolatility_(S:list, K:list, r:list, q:list, T:list, Vol_guess:list, market_price:list) -> list:
    return _red_pandas.CallImpliedvolatility(array(S), array(K), array(r), array(q), array(T), array(Vol_guess), array(market_price)).to_vector()

@excelbind.function
def PutImpliedvolatility_(S:list, K:list, r:list, q:list, T:list, Vol_guess:list, market_price:list) -> list:
    return _red_pandas.PutImpliedvolatility(array(S), array(K), array(r), array(q), array(T), array(Vol_guess), array(market_price)).to_vector()

@excelbind.function
def GreeksVolga(S:list, K:list, r:list, q:list, T:list, Vol:list, d_uno:list, d_dos:list) -> list:
    return _red_pandas.GreeksVolga(array(S), array(K), array(r), array(q), array(T), array(Vol), array(d_uno), array(d_dos)).to_vector()
