
ifeq ($(OS),Windows_NT)
LDFLAGS := $(LDFLAGS) -lws2_32 -lboost_unit_test_framework-mt
CXXFLAGS := $(CXXFLAGS) -D_WIN32_WINNT=0x0600 -DWINVER=0x0600
else
LDFLAGS := $(LDFLAGS) -ldl -lboost_unit_test_framework
endif

# FIXME fa schifo.
EXT_OBJ := client_[^m]*.o
