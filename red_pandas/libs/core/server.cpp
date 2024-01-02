//
// Created by n424613 on 02/01/2024.
//
#pragma once

#include <iostream>
#include <string>

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class handler : public std::enable_shared_from_this<handler>
{
public:
    handler(tcp::socket socket)  : m_socket(std::move(socket)) { }

    void run() {
        wait_for_request();
    }
private:
    void wait_for_request() {
        auto self(shared_from_this());
        boost::asio::async_read_until(m_socket, m_buffer, "\0",
                                      [this, self](boost::system::error_code ec, std::size_t /*length*/)
                                      {
                                          if (!ec)  {
                                              std::string data{
                                                      std::istreambuf_iterator<char>(&m_buffer),
                                                      std::istreambuf_iterator<char>()
                                              };
                                              std::cout << data << std::endl;
                                              // string to data

                                              wait_for_request();
                                          } else {
                                              // maybe client disconnection
                                              // std::cout << "error server 2: " << ec << std::endl;
                                          }
                                      });
    }
private:
    tcp::socket m_socket;
    boost::asio::streambuf m_buffer;
};


class server
{
public:
    server(boost::asio::io_context& io_context, short port)
            : m_acceptor(io_context, tcp::endpoint(tcp::v4(), port))
    {
        do_accept();
    }
private:
    void do_accept() {
        m_acceptor.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::cout << "creating handler on: "
                          << socket.remote_endpoint().address().to_string()
                          << ":" << socket.remote_endpoint().port() << '\n';

                std::make_shared<handler>(std::move(socket))->run();
            } else {
                std::cout << "error server1: " << ec.message() << std::endl;
            }
            do_accept();
        });
    }
private:
    tcp::acceptor m_acceptor;
};


int main(int argc, char* argv[])
{
    boost::asio::io_context io_context;
    server s(io_context, 25000);
    io_context.run();

    return 0;
}
