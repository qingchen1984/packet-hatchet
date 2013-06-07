CC=gcc
LDLIBS=-largtable2 -lpcap
FLAGS=-Wall

all:
	$(CC) $(FLAGS) -o ph src/*.c $(LDLIBS)
clean:
	rm -f src/*.o ph
