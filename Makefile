# Makefile:
# 03/07/2019 by Mohammad Gagai

all: client server

clean:
	rm -rf client server

client: client.c
	gcc client.c -Wall -lpthread -ansi -std=c99 -o client

server: server.c
	gcc server.c -Wall -ansi -std=c99 -o server

# EOF
