CFLAGS=-Wall -g -DNDEBUG

all:        client

client: 	../shared/util.o client.o
			gcc ../shared/util.o client.o -o client
clean:
			rm -f ../shared/util.o client.o
