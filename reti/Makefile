CC=gcc
CFLAGS=-Wall

EXE=client server

all: $(EXE)

client: 01_client.c
	$(CC) $(CFLAGS) $^ -o $@

server: 01_server.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm $(EXE)
