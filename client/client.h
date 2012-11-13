
#include <control/pid.h>
#include "connection.h"

class Client{
	private:
		Connection* conn;
		PID* pid;
	public:
		Client(Connection*, PID*);	
		void work();
};
