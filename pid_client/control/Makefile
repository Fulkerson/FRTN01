
LIB_NAME=control
OBJS= pid.o
HEAD= pid.h

all: $(OBJS)
	ar rcs lib$(LIB_NAME).a $+

test: all test.o 
	$(CXX) -o test test.o -L. -l$(LIB_NAME)
	./test
	rm -f test

clean:
	rm -f *.o *.a

