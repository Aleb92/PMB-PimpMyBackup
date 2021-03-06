# Per prima cosa le flags globali

PMB_VERSION := $(shell git rev-parse HEAD | cut -c1-10)

# Prima il c++
export CXXFLAGS := -g -std=c++11 -DPMB_VERSION=\"$(PMB_VERSION)\"

# Poi il linker!
export LDFLAGS := -lsqlite3 -lcrypto -lpthread

export EXT_OBJ := 

# Ok ogni makefile farà poi quello che deve per aggiungere quello che serve.
# Adesso, ogni sottocartella corrisponde ad un target.
# Con questo prendo tutti i nomi delle cartelle
TARGETS := $(notdir $(wildcard source/[!t]*)) 

# Ok adesso eseguo all, che l'unica cosa che fa è dipendere da tutti i target.
all: $(TARGETS)
	

# Ogni target a sua volta chiama il costruttore vero nella cartella build
# e dipende da tutti i sorgenti presenti nella cartella di destinazione.
%: source/%/*
	$(MAKE) -C build $@

# clean è da eseguire sempre.
.PHONY: clean


clean: clean-db
	$(RM) -rf build/[!M]*

clean-db:
	$(RM) -rf server.db build/test

database:
	sqlite3 server.db < create_db.sql
	./create_user.sh root toor
	./create_user.sh hhh ppp

restart_client:
	$(RM) -rf build/test* build/temp client.log client.log.old
	
doc:
	doxygen doxygen.conf