
# Uncomment below line for raspbian. Hard float is faster.
#CXXFLAGS += -mfloat-abi=hard

BIN_NAME= client
PROTO_DIR = ../proto

# Define if not defined
INCLUDE_PATH?=../include
LIB_PATH?=../lib
BIN_PATH?=../bin

CXXFLAGS += -std=c++0x -pipe -O0 -g -Wall -W -pedantic-errors
CXXFLAGS += -Wmissing-braces -Wparentheses

CPPFLAGS+= -I$(INCLUDE_PATH)

LDFLAGS+= -L$(LIB_PATH) -lcontrol -lprotobuf -lboost_system
LDFLAGS+= -lboost_thread-mt -lrt

OBJS= batchtank.pb.o main.o

all: $(OBJS)
	$(CXX) -o $(BIN_NAME) $+ $(LDFLAGS) 

install:
	cp $(BIN_NAME) $(BIN_PATH)

uninstall:
	rm -f $(BIN_PATH)/$(BIN_NAME)

clean: protoclean
	rm -f *.o $(BIN_NAME)

main.o: main.cc batchtank.pb.h ../common/message_utils.h
	$(CXX) -c $(CXXFLAGS) main.cc -o main.o $(CPPFLAGS)

batchtank.pb.o: batchtank.pb.cc
	$(CXX) -c $(CXXFLAGS) batchtank.pb.cc -o batchtank.pb.o

batchtank.pb.h: batchtank.pb.cc

batchtank.pb.cc: $(PROTO_DIR)/batchtank.proto
	protoc -I=$(PROTO_DIR) $(PROTO_DIR)/batchtank.proto --cpp_out=.

protoclean: clean
	rm -rf batchtank.pb.h batchtank.pb.cc
