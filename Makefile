CC=gcc
FLAGS=-Wall -g

all: stree 

ftw1: ftw1.c
	$(CC) $(FLAGS) $^ -o $@

ftw2: ftw2.c
	$(CC) $(FLAGS) $^ -o $@

stree: stree.c
	$(CC) $(FLAGS) $^ -o $@

clean:
	rm stree ftw1 ftw2  