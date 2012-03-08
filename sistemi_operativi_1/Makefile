CC=gcc
CFLAGS=-Wall

EXE=fork shell

all: $(EXE)

fork: fork.c
	$(CC) $(CFLAGS) $^ -o $@

shell: shell.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm $(EXE)
