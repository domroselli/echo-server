CFLAGS=-Wall -g -DNDEBUG

all:        server

server: 	../shared/util.o server.o
			gcc ../shared/util.o server.o -o server
clean:
		rm -f ../shared/util.o server.o
