ifeq ($(OS),Windows_NT)
LDFLAGS := $(LDFLAGS) -lws2_32
else
export SKIP := 1
endif
