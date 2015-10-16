ifeq ($(OS),Windows_NT)
LDFLAGS := $(LDFLAGS) -lws2_32
else
SKIP := 
endif
