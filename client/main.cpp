#include "client.h"
#include "connection.h"


int main(){
	std::string u = "poop";
	int p = 1234;

	// parse from args here

	Connection* conn = new Connection();
	PID* pid = new PID(1,1,1,1,1,1);	
	conn->setUp(u,p);
	Client* c = new Client(conn, pid);
	c->work();
}
