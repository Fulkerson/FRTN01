
# Define if not defined
INCLUDE_PATH?=../include
LIB_PATH?=../lib

CC=g++
CPPFLAGS= -I$(INCLUDE_PATH)
LDFLAGS= -L$(LIB_PATH) -lcontrol

BIN_NAME= client
OBJS= client.o

all: $(OBJS) main.o
	$(CC) -o $(BIN_NAME) $+

install: all
	cp $(BIN_NAME) $(BIN_PATH)

uninstall:
	rm $(BIN_PATH)/$(BIN_NAME)

test: all test.o 
	$(CC) -o test $(OBJS) test.o 
	./test
	rm test

clean:
	rm *.o $(BIN_NAME)

