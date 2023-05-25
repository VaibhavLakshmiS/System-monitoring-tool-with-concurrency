#this is a makefile 

#compiler flags 
CC = gcc
CFLAGS = -Wall -Werror 
LIBS = -lm
all:stats

stats: A3main.o stats_functions.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
%.o: %.c
	$(CC) $(CFLAGS)-c $<

.PHONY: clean

clean:
	rm *.o