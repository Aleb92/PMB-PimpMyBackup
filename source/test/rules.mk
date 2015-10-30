
ifeq ($(OS),Windows_NT)
LDFLAGS := $(LDFLAGS) -lws2_32
else
LDFLAGS := $(LDFLAGS) -ldl
endif

# FIXME fa schifo.
LDFLAGS := client_[^m]*.o $(LDFLAGS) -lboost_unit_test_framework
