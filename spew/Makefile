CC=gcc
CFLAGS=-Wall -std=c11
SOURCES=$(wildcard src/*.c)
OBJECTS=$(patsubst src/%.c, src/%.o, $(SOURCES))

main: $(OBJECTS)
	$(CC) $(CFLAGS) -O3 -o $@ $(OBJECTS)	

debug:
	$(CC) $(CFLAGS) -ggdb -o $@ $(SOURCES)

$(OBJECTS): %.o : %.c
	$(CC) $(CFLAGS) -O3 -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJECTS) main debug
