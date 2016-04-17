ifeq ($(OS),Windows_NT)
LDFLAGS := $(LDFLAGS) -lws2_32
else
LDFLAGS := $(LDFLAGS) -ldl
endif

LDFLAGS := -static $(LDFLAGS) 