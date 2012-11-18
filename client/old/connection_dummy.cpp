
#include "connection.h"
#include <iostream>

int ctr = 0;

Connection::Connection(std::string u, int p){
	this->url = u;
	this->port = p;
	std::cout << "This is a dummy connection." << "\n";
	std::cout << "Url:" << this->url <<  " Port:" << this->port  << "\n";
}

bool Connection::isUp(){
	return ctr++ < 5;
}

void Connection::send(double u){
	std::cout << "sending " << u << "\n";
}

double Connection::receive(){
	sleep(1);
	std::cout << "returning " << 1 << "\n";
	return 1;
}

