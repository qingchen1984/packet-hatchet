CC=gcc
LDLIBS=-largtable2
FLAGS=

all:
	$(CC) $(FLAGS) $(LDLIBS) -o ph src/packet_hatchet.c
