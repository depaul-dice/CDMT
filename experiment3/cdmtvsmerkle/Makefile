CC = gcc
CFLAGS = -O3 -Wall -std=c11
CFLAGS_DEBUG = -Wall -std=c11
DEBUG = -g -ggdb
LIBRARY = -lssl -lcrypto

SOURCES = $(wildcard %.c)
HEADER = $(wildcard %.h)
OBJECTS = $(patsubst %.c, %.o,$(wildcard *.c))
EXECUTABLE=main debug extra

all: main debug

extra: 
	$(CC) extra.c parse.c pos.c merkle.c node.c hashmap.c queue.c $(CFLAGS) -o $@ $(LIBRARY)

main: $(SOURCES) $(HEADER) 
	$(CC) main.c parse.c pos.c merkle.c node.c hashmap.c queue.c $(CFLAGS) -o $@ $(LIBRARY)

debug: 
	$(CC) $(CFLAGS_DEBUG) $(DEBUG) main.c parse.c pos.c merkle.c node.c hashmap.c queue.c -o $@ $(LIBRARY) 

.PHONY: clean
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: clean_all
clean_all:
	rm -f $(OBJECTS) $(EXECUTABLE) merkleposcompare.csv posmeta/* 
