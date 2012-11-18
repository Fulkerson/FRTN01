
#include "client.h"


Client::Client(std::string u, int p, PID* pid){
	this->conn = new Connection(u,p);
	this->pid = pid;
}

void Client::work(){
	double y = 0;
	double u = 0;
	while (this->conn->isUp()) {
		y = this->conn->receive();
		u = this->pid->next(y);
		this->conn->send(u);
	}
}


