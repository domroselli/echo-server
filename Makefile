CFLAGS=-Wall -g -DNDEBUG

all:         echo_util echo_server echo_client

echo_util:	 echo_util.o

echo_server: echo_server.o

echo_client: echo_client.o

clean:
		rm -f echo_server.o echo_client.o echo_util.o
