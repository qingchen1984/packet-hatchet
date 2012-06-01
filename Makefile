CC=gcc
LDLIBS=-largtable2
FLAGS=

all:
	$(CC) $(FLAGS) $(LDLIBS) -o ph src/*.c
