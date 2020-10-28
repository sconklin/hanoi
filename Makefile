CC=gcc 
CFLAGS=-Wall -fPIC
LDFLAGS=-lncurses

.PHONY: all
all: hanoi
hanoi.o: hanoi.c hanoi.h display.h
display.o: display.c hanoi.h display.h

OBJECTS=hanoi.o display.o
hanoi: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o hanoi $(LDFLAGS)

clean:
	rm -f hanoi hanoi.o display.o
run: hanoi
	./hanoi
