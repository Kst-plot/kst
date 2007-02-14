CC = gcc -g

all: replay

replay: replay.o getdata.o getdata.h getdata_struct.h
	$(CC) -o replay replay.o getdata.o -lm

