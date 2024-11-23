CC = gcc
CFLAGS = -Wall -Wextra

.PHONY: all clean

all: shell

shell: shell.c
	$(CC) $(CFLAGS) -o shell shell.c

clean:
	rm -f shell

run: shell
	./shell
	@echo "Shell ended"
