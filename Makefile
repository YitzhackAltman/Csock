CFLAGS=-Wall -Wextra -std=c11 -pedantic

soc: ctro.c
	$(CC) $(CFLAGS) -o soc ctro.c