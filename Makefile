CC=gcc
LDLIBS=-largtable2
FLAGS=

all:
	$(CC) $(FLAGS) $(LDLIBS) -o ph src/packet_hatchet.c src/ip_factory.c src/bouncer.c
