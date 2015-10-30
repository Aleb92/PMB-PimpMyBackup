ifeq ($(OS),Windows_NT)
LDFLAGS := $(LDFLAGS) -lws2_32
CXXFLAGS := $(CXXFLAGS) -DUNICODE 
else
export SKIP := 1
endif
