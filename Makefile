# Makefile:
# 10/27/2018 by Mohammad Gagai

all: client server

clean:
	rm -rf client server

client: client.c
	gcc client.c -Wall -ansi -std=c99 -o client

server: server.c
	gcc server.c -Wall -ansi -std=c99 -o server

# EOF
