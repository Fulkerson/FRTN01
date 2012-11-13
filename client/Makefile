
# Define if not defined
INCLUDE_PATH?=../include
LIB_PATH?=../lib

CC=g++
CPPFLAGS= -I$(INCLUDE_PATH)
LDFLAGS= -L$(LIB_PATH) -lcontrol

BIN_NAME= client
OBJS= client.o

all: $(OBJS) connection.o main.o 
	$(CC) $(LDFLAGS) -o $(BIN_NAME) $+

install: all
	cp $(BIN_NAME) $(BIN_PATH)

uninstall:
	rm $(BIN_PATH)/$(BIN_NAME)

test: $(OBJS) connection_dummy.o main.o 
	$(CC) -o test $+ $(LDFLAGS)
	./test url123url 1234
#	rm test

clean:
	rm *.o $(BIN_NAME)

