#include <iostream>
#include <thread>
#include <string>
#include <boost/asio.hpp>

#include "server.h"
#include "batchtank.pb.h"

using boost::asio::ip::tcp;

Connection::Connection(tcp::socket* socket) : 
    socket(socket) {}

void
Connection::run()
{
    try {
        std::string message("Hello World!\n");
        boost::system::error_code ignored_error;
        boost::asio::write(*socket, boost::asio::buffer(message),
                           ignored_error);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    delete this;
}

Connection::~Connection() 
{
    delete socket;
}


int
main()
{
    try {
        boost::asio::io_service io_service;
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 54000));

        Connection* conn;

        while(true) {
            tcp::socket* socket = new tcp::socket(io_service);
            acceptor.accept(*socket);

            conn = new Connection(socket);
            std::thread t(&Connection::run, conn);
            t.detach();

       }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
