#include "client.h"
#include "connection.h"
#include <iostream>
#include <stdlib.h>

int main(int argc, char* argv[]){
	std::string u = std::string("localhost");
	int p = 1234;
	double pp[] = {1,1,1,1,1,1};	

	if (argc == 2) {
		std::cout << "File parsing not implemented\n"
			<< "Using standard url:port\n";
	}
	if (argc == 9) {
		u = std::string(argv[1]);
		p = atoi(argv[2]);
		for(int i = 0; i < 6; i++)
			pp[i] = atoi(argv[i+3]);
		std::cout << "Parsed command line parameters:\n";
	}
	
	std::cout << "\t" << u << ":" << p << "\n";
	for(int i = 0; i < 6; i++)
		std::cout << "\t" << pp[i];
	std::cout << "\n";
	
	Connection* conn = new Connection();
	PID* pid = new PID(pp[0],pp[1],pp[2],pp[3],pp[4],pp[5]);	
	conn->setUp(u,p);
	Client* c = new Client(conn, pid);
	c->work();
}
