# Questa è la cartella dei sorgenti
S_FOLDER := ../source/$(MAKECMDGOALS)
INCLUDE := $(INCLUDE) -I../source -I$(S_FOLDER)/include 

CFLAGS := -O3 -Os

ifneq ("$(wildcard $(S_FOLDER)/rules.mk)","")
	include $(S_FOLDER)/rules.mk
endif

CXX_FILES := $(notdir $(wildcard $(S_FOLDER)/*.cpp))
OBJ_FILES := $(addprefix $(MAKECMDGOALS)_,$(addsuffix .o,$(CXX_FILES:.cpp=)))


ifneq ($(MAKECMDGOALS),utilities)

$(MAKECMDGOALS): $(OBJ_FILES) util
	$(CXX) $(OBJ_FILES) utilities_*.o sqlite.o -o $(MAKECMDGOALS) $(LDFLAGS)

else

$(MAKECMDGOALS): $(OBJ_FILES) sqlite.o
	
endif

sqlite.o: $(S_FOLDER)/sqlite3.c
	$(CC) $(CFLAGS) $(S_FOLDER)/sqlite3.c -c -o sqlite.o 

util: 
	$(MAKE) utilities

$(MAKECMDGOALS)_%.o: $(S_FOLDER)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@ $? 
