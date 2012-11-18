
#ifndef CLIENT_H
#define CLIENT_H

#include <control/pid.h>
#include "connection.h"

class Client{
	private:
		Connection* conn;
		PID* pid;
	public:
		Client(std::string, int, PID*);	
		void work();
};

#endif
