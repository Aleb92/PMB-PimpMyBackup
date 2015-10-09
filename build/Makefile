# Questa è la cartella dei sorgenti
S_FOLDER := ../source/$(MAKECMDGOALS)
INCLUDE := $(INCLUDE) -I../source -I$(S_FOLDER)/include 

CFLAGS := -O3 -Os -lpthread 

ifneq ("$(wildcard $(S_FOLDER)/rules.mk)","")
	include $(S_FOLDER)/rules.mk
endif

CXX_FILES := $(notdir $(wildcard $(S_FOLDER)/*.cpp $(S_FOLDER)/*/*.cpp))
OBJ_FILES := $(addsuffix .o,$(CXX_FILES:.cpp=))


ifneq ($(MAKECMDGOALS),utilities)

$(MAKECMDGOALS): $(OBJ_FILES) utilities
	$(CXX) $(LDFLAGS) $(OBJ_FILES) -o $(MAKECMDGOALS) 

endif

utilities: $(OBJ_FILES) 
	$(CC) $(CFLAGS) -c -o sqlite.o 

%.o: $(S_FOLDER)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@ $?
