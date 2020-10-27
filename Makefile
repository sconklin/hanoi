CC=gcc 
CFLAGS=-Wall -fPIC

all: hanoi
hanoi: hanoi.o display.o
hanoi.o: hanoi.c hanoi.h display.h
display.o: display.c hanoi.h display.h

clean:
	rm -f hanoi hanoi.o display.o
run: hanoi
	./hanoi
