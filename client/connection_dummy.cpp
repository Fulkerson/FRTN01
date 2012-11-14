
#include "connection.h"
#include <iostream>

int ctr = 0;

Connection::Connection(){
	
}

void Connection::setUp(const std::string url, int port){
	std::cout << "This is a dummy connection." << "\n";
	std::cout << "Url:" << url <<  " Port:" << port  << "\n";
}

bool Connection::isUp(){
	return ctr++ < 5;
}

double Connection::sendAndReceive(double u){
	sleep(1);
	std::cout << "sending " << u << " returning " << u + 1 << "\n";
	return u+1;
}

