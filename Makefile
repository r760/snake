# makefile for snake
#
# make snake

CC=g++
CFLAGS=-lncurses

clean:
	rm -rf snake

snake: snake.cpp
	$(CC) $(CFLAGS) snake.cpp -o snake
