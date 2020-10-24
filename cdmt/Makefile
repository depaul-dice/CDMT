CC = gcc
CFLAGS = -O3 -Wall -std=c11
CFLAGS_DEBUG = -Wall -std=c11
DEBUG = -g -ggdb
LIBRARY = -lssl -lcrypto -lrt

SOURCES = $(wildcard %.c)
HEADER = $(wildcard %.h)
OBJECTS = $(patsubst %.c, %.o,$(wildcard *.c))
EXECUTABLE = main debug

all: debug main

main:
	$(CC) $(CFLAGS) main.c parse.c pos.c queue.c node.c hashmap.c -o $@ $(LIBRARY)

#main: $(SOURCES) $(HEADER)
#$(CC) $(CFLAGS) $(SOURCES) -o $@ $(LIBRARY)

debug: 
	$(CC) $(CFLAGS_DEBUG) $(DEBUG) main.c parse.c pos.c queue.c node.c hashmap.c -o $@ $(LIBRARY) 

.PHONY: clean
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: clean_all
clean_all:
	rm -f $(OBJECTS) $(EXECUTABLE) posmeta/*
