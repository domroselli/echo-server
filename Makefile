CFLAGS=-Wall -g -DNDEBUG

all:         util server client

util:	 	 shared/util.o

server:      shared/util.o server/server.o

client: 	 shared/util.o client/client.o

clean:
		rm -f shared/util.o server/server.o client/client.o
