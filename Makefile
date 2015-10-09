# Per prima cosa le flags globali

# Prima il c++
export CXXFLAGS := -g -std=c++11

# Poi il linker!
export LDFLAGS := -lpthread -ldl

# Ok ogni makefile farà poi quello che deve per aggiungere quello che serve.
# Adesso, ogni sottocartella corrisponde ad un target.
# Con questo prendo tutti i nomi delle cartelle
TARGETS := $(notdir $(wildcard source/*)) 

# Ok adesso eseguo all, che l'unica cosa che fa è dipendere da tutti i target.
all: $(TARGETS)
	

# Ogni target a sua volta chiama il costruttore vero nella cartella build
# e dipende da tutti i sorgenti presenti nella cartella di destinazione.
%: source/%/*
	$(MAKE) -C build $@

# clean è da eseguire sempre.
.PHONY: clean


clean:
	$(RM) -rf build/[!M]*
