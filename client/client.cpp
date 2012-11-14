
#include "client.h"


Client::Client(Connection* conn, PID* pid){
	this->conn = conn;
	this->pid = pid;
}

void Client::work(){
	double y = 0;
	double u = 0;
	while (this->conn->isUp()) {
		y = this->conn->sendAndReceive(u);
		u = this->pid->next(y);
	}
}


