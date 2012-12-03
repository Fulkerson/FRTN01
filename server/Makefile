CXXFLAGS += -std=c++0x -pipe -O0 -g -Wall -W -pedantic-errors
CXXFLAGS += -Wmissing-braces -Wparentheses

# Uncomment below line for raspbian. Hard float is faster.
#CXXFLAGS += -mfloat-abi=hard

# Uncomment below line for periodic polling of the process to be used.
#CXXFLAGS += -DPOLLING_UPDATE

LDLIBS = -lprotobuf -lboost_system -lboost_thread-mt -lrt
PROTO_DIR = ../proto

BIN_PATH?=../bin

all: batchtank_server

install: all
	cp batchtank_server $(BIN_PATH) 

uninstall:
	rm -f $(BIN_PATH)/batchtank_server

batchtank_server: batchtank_server.o batchtank.pb.o cook.o
	$(CXX) $(CXXFLAGS) batchtank_server.o batchtank.pb.o cook.o $(LDLIBS) \
		-o batchtank_server

batchtank_server.o: batchtank_server.cc batchtank.pb.h ../common/message_utils.h
	$(CXX) -c $(CXXFLAGS) batchtank_server.cc -o batchtank_server.o

cook.o: ../cook/cook.c
	$(CXX) -c $(CXXFLAGS) ../cook/cook.c -o cook.o

batchtank.pb.o: batchtank.pb.cc
	$(CXX) -c $(CXXFLAGS) batchtank.pb.cc -o batchtank.pb.o

batchtank.pb.h: batchtank.pb.cc

batchtank.pb.cc: $(PROTO_DIR)/batchtank.proto
	protoc -I=$(PROTO_DIR) $(PROTO_DIR)/batchtank.proto --cpp_out=.

clean:
	rm -rf batchtank_server *.o

protoclean: clean
	rm -rf batchtank.pb.h batchtank.pb.cc
