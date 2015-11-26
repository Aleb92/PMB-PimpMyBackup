
ifeq ($(OS),Windows_NT)
LDFLAGS := $(LDFLAGS) -lws2_32
CXXFLAGS := $(CXXFLAGS) -D_WIN32_WINNT=0x0600 -DWINVER=0x0600
else
LDFLAGS := $(LDFLAGS) -ldl
endif

# FIXME fa schifo.
EXT_OBJ := client_[^m]*.o
LDFLAGS := $(LDFLAGS) -lboost_unit_test_framework
