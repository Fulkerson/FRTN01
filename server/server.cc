#include <iostream>
#include <thread>
#include <string>
#include <boost/asio.hpp>

#include "server.h"
#include "batchtank.pb.h"

using boost::asio::ip::tcp;


void
hello()
{
    std::cout << "Hello from thread " << std::endl;
}


int
main()
{
    try {
        boost::asio::io_service io_service;
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 54000));

        while(true) {
            tcp::socket socket(io_service);
            acceptor.accept(socket);

            std::string message("Hello World!");

            boost::system::error_code ignored_error;
            boost::asio::write(socket, boost::asio::buffer(message),
                               ignored_error);
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
