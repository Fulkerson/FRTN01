
#include "pid.h"
#include <iostream>
#include <assert.h>

int main(){
	PID* pid = new PID(3,2,1,5,-5,5);
	for (int i = 0; i < 10; i++){
		assert(pid->next(6));
		assert(pid->next(4));
	}
}

