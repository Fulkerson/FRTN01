CXXFLAGS += -std=c++0x -pipe -O2 -Wall -W -pedantic-errors
CXXFLAGS += -Wmissing-braces -Wparentheses

# Uncomment below line for raspbian. Hard float is faster.
#CXXFLAGS += -mfloat-abi=hard

# Uncomment below line for periodic polling of the process to be used.
#CXXFLAGS += -DPOLLING_UPDATE

LDLIBS = -lprotobuf-lite -lboost_system -lboost_thread-mt -lrt
PROTO_DIR = ../proto

BIN_PATH?=../bin

all: batchtank_server

install: all
	cp batchtank_server $(BIN_PATH) 

uninstall:
	rm -f $(BIN_PATH)/batchtank_server

dummy_batchtank_server: dummy_batchtank_server.o batchtank.pb.o
	$(CXX) $(CXXFLAGS) dummy_batchtank_server.o batchtank.pb.o \
	       	$(LDLIBS) -o dummy_batchtank_server

dummy_batchtank_server.o: batchtank_server.cc ../common/batchtank.pb.h \
       dummy_cook.h ../common/message_utils.h
	$(CXX) -c $(CXXFLAGS) -DDUMMY_PROCESS batchtank_server.cc \
	       	-o dummy_batchtank_server.o

batchtank_server: batchtank_server.o batchtank.pb.o cook.o
	$(CXX) $(CXXFLAGS) batchtank_server.o batchtank.pb.o cook.o $(LDLIBS) \
		-o batchtank_server

batchtank_server.o: batchtank_server.cc ../common/batchtank.pb.h \
	../common/message_utils.h
	$(CXX) -c $(CXXFLAGS) batchtank_server.cc -o batchtank_server.o

cook.o: ../cook/cook.c
	$(CXX) -c $(CXXFLAGS) ../cook/cook.c -o cook.o

batchtank.pb.o: ../common/batchtank.pb.cc
	$(CXX) -c $(CXXFLAGS) ../common/batchtank.pb.cc -o batchtank.pb.o

clean:
	rm -rf batchtank_server dummy_batchtank_server *.o
