CXX       = g++
CXXFLAGS  = -std=c++11 -pipe -O0 -g -Wall -W -pedantic-errors
CXXFLAGS += -Wmissing-braces -Wparentheses
# Uncomment below line for raspbian. Hard float is faster.
#CXXFLAGS += -mfloat-abi=hard
LDLIBS = -lprotobuf-lite -lboost_system -lpthread
PROTO_DIR = ../proto

all: server

server: server.o batchtank.pb.o
	$(CXX) $(CXXFLAGS) server.o batchtank.pb.o $(LDLIBS) -o server

server.o: server.cc batchtank.pb.h
	$(CXX) -c $(CXXFLAGS) server.cc -o server.o

batchtank.pb.o: batchtank.pb.cc
	$(CXX) -c $(CXXFLAGS) batchtank.pb.cc -o batchtank.pb.o

batchtank.pb.h: batchtank.pb.cc

batchtank.pb.cc: $(PROTO_DIR)/batchtank.proto
	protoc -I=$(PROTO_DIR) $(PROTO_DIR)/batchtank.proto --cpp_out=.

clean:
	rm -rf server *.o

protoclean:
	rm -rf batchtank.pb.h batchtank.pb.cc
