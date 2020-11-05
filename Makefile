CC = gcc
CFLAGS = -g -Wall

default: websocket

websocket:  websocket.o
	$(CC) -o websocket ./obj/websocket.o

ALLFLAGS = $(CFLAGS)
websocket.o: ./src/websocket.c
	$(CC) $(ALLFLAGS) -c ./src/websocket.c -o ./obj/websocket.o
