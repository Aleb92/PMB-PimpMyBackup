
CXXFLAGS = -g -std=c++11
ifeq ($(OS),Windows_NT)
	LDFLAGS = -lpthread -lws2_32
else
	LDFLAGS = -lpthread
endif

all: socket

socket: 
	$(CXX) $(CXXFLAGS) socket.cpp ../utilities/socket.cpp -o ../../build/test/sock_test $(LDFLAGS)
