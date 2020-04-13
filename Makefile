.PHONY: all clean
CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic -pthread -lrt

EXECUTABLE = proj2

all: $(EXECUTABLE)

proj2: $(EXECUTABLE).o
	$(CC) $(CFLAGS) -o $@ $^

proj2.o: $(EXECUTABLE).c
	$(CC) $(CFLAGS) -c $^

zip:
	zip xchalu15.zip *.c *.h Makefile

clean:
	rm -rf $(EXECUTABLE) *.o
