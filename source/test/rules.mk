
ifeq ($(OS),Windows_NT)
LDFLAGS := $(LDFLAGS) -lws2_32
else
LDFLAGS := $(LDFLAGS) -ldl
endif

LDFLAGS := $(LDFLAGS) -lboost_unit_test_framework-mt