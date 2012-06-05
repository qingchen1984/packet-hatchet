CC=gcc
LDLIBS=-largtable2 -lpcap
FLAGS=

all:
	$(CC) $(FLAGS) $(LDLIBS) -o ph src/*.c
