.PHONY: all clean
CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic

EXECUTABLE = proj2

all: $(EXECUTABLE)

proj2: $(EXECUTABLE).o
	$(CC) $(CFLAGS) -o $@ $^ -pthread -lrt

proj2.o: $(EXECUTABLE).c
	$(CC) $(CFLAGS) -c $^ -pthread -lrt

zip:
	zip $(EXECUTABLE).zip *.c *.h Makefile

clean:
	rm -rf $(EXECUTABLE) *.o
