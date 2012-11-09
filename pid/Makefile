
$(CC)=g++

all: pid.o
	ar rcs libpid.a pid.o

test: all test.o 
	g++ -o test test.o -L. -lpid
	./test

clean:
	rm *.o *.a test 2>/dev/null || return 0
