# Questa è la cartella dei sorgenti
S_FOLDER := ../source/$(MAKECMDGOALS)
INCLUDE := $(INCLUDE) -I../source -I$(S_FOLDER)/include 

ifneq ("$(wildcard $(S_FOLDER)/rules.mk)","")
	include $(S_FOLDER)/rules.mk
endif

CXX_FILES := $(notdir $(wildcard $(S_FOLDER)/*.cpp))
OBJ_FILES := $(addprefix $(MAKECMDGOALS)_,$(addsuffix .o,$(CXX_FILES:.cpp=)))

ifneq ($(MAKECMDGOALS),utilities)

ifndef SKIP
$(MAKECMDGOALS): $(OBJ_FILES) util
	$(CXX) $(OBJ_FILES) $(EXT_OBJ) utilities_*.o -o $(MAKECMDGOALS) $(LDFLAGS)
else
$(MAKECMDGOALS):
endif

else

$(MAKECMDGOALS): $(OBJ_FILES)
	
endif

util: 
	$(MAKE) utilities

$(MAKECMDGOALS)_%.o: $(S_FOLDER)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@ $? 
