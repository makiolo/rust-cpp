//
// Created by n424613 on 02/01/2024.
//

#include <iostream>
#include <string>
#include <boost/asio.hpp>


using boost::asio::ip::tcp;
using asio_context = boost::asio::io_context;


class scoped_connection
{
public:
    scoped_connection(asio_context& context)
        : _socket(context)
        , _resolver(context)
    {
        ;
    }

    ~scoped_connection()
    {
        boost::system::error_code ec;
        _socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        _socket.close();
    }

    void connect(const std::string& hostname, const std::string& port)
    {
        boost::asio::connect(_socket, _resolver.resolve(hostname, port));
    }

    void write(const std::string& data)
    {
        auto result = boost::asio::write(_socket, boost::asio::buffer(data));
        std::cout << "data sent: " << data.length() << '/' << result << std::endl;
    }

protected:
    tcp::socket _socket;
    tcp::resolver _resolver;
};

int main(int argc, char* argv[])
{ 
    // Implementar calculadora con shunting yard algorithmn
    // https://github.com/aslze/asl-calculator?tab=readme-ov-file
    //
    asio_context io_context;

    scoped_connection conn(io_context);
    conn.connect("127.0.0.1", "25000");

    std::string data{"some client data ..."};
    conn.write(data);

    return 0;
}
