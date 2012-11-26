CXXFLAGS += -std=c++0x -pipe -O0 -g -Wall -W -pedantic-errors
CXXFLAGS += -Wmissing-braces -Wparentheses
# Uncomment below line for raspbian. Hard float is faster.
#CXXFLAGS += -mfloat-abi=hard
LDLIBS = -lprotobuf -lboost_system -lboost_thread-mt -lrt
PROTO_DIR = ../proto

all: example_client

example_client: example_client.o batchtank.pb.o
	$(CXX) $(CXXFLAGS) example_client.o batchtank.pb.o $(LDLIBS) \
		-o example_client

example_client.o: example_client.cc batchtank.pb.h ../common/message_utils.h
	$(CXX) -c $(CXXFLAGS) example_client.cc -o example_client.o

batchtank.pb.o: batchtank.pb.cc
	$(CXX) -c $(CXXFLAGS) batchtank.pb.cc -o batchtank.pb.o

batchtank.pb.h: batchtank.pb.cc

batchtank.pb.cc: $(PROTO_DIR)/batchtank.proto
	protoc -I=$(PROTO_DIR) $(PROTO_DIR)/batchtank.proto --cpp_out=.

clean:
	rm -rf example_client *.o

protoclean: clean
	rm -rf batchtank.pb.h batchtank.pb.cc
