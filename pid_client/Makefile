
# Uncomment below line for raspbian. Hard float is faster.
#CXXFLAGS += -mfloat-abi=hard

BIN_NAME= pid_client
PROTO_DIR = ../proto

INCLUDE_PATH=.
LIB_PATH=control
# Define if not defined
BIN_PATH?=../bin

CXXFLAGS += -std=c++0x -pipe -O2 -Wall -W -pedantic-errors
CXXFLAGS += -Wmissing-braces -Wparentheses

CPPFLAGS+= -I$(INCLUDE_PATH)

LDFLAGS+= -L$(LIB_PATH) -lcontrol -lprotobuf-lite -lboost_system
LDFLAGS+= -lboost_thread-mt -lrt

OBJS= batchtank.pb.o main.o
SUB_DIRS= control

all: $(OBJS)
	for DIR in $(SUB_DIRS); do \
		cd $(PWD)/$$DIR && make; \
	done
	$(CXX) -o $(BIN_NAME) $(OBJS) $(LDFLAGS) 

install:
	cp $(BIN_NAME) $(BIN_PATH)

uninstall:
	rm -f $(BIN_PATH)/$(BIN_NAME)

clean:
	for DIR in $(SUB_DIRS); do \
		cd $(PWD)/$$DIR && make clean; \
	done
	rm -f *.o $(BIN_NAME)

main.o: main.cc ../common/batchtank.pb.h ../common/message_utils.h
	$(CXX) -c $(CXXFLAGS) main.cc -o main.o $(CPPFLAGS)

batchtank.pb.o: ../common/batchtank.pb.cc
	$(CXX) -c $(CXXFLAGS) ../common/batchtank.pb.cc -o batchtank.pb.o
