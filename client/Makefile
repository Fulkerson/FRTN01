
# Define if not defined
INCLUDE_PATH?=../include
LIB_PATH?=../lib

CPPFLAGS= -I$(INCLUDE_PATH)
LDFLAGS= -L$(LIB_PATH) -lcontrol

BIN_NAME= client
OBJS= client.o

all: $(OBJS) connection.o main.o 
	$(CXX) $(LDFLAGS) -o $(BIN_NAME) $+

install:
	cp $(BIN_NAME) $(BIN_PATH)

uninstall:
	rm -f $(BIN_PATH)/$(BIN_NAME)

test: $(OBJS) connection_dummy.o main.o 
	$(CXX) -o test $+ $(LDFLAGS)
	./test localhost 1337 3 2 1 1 -5 5
	rm -f test

clean:
	rm -f *.o $(BIN_NAME)

