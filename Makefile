CC=gcc
FLAGS=-Wall -g

all: stree

stree: stree.c
	$(CC) $(FLAGS) $^ -o $@

clean:
	rm -rf stree