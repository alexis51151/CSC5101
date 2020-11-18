FLAGS=-g
LDFLAGS=-lpthread


all: rw

rw: rw.o
	$(CC) -o rw rw.o $(FLAGS) $(LDFLAGS)

rw.o: rw.c
	$(CC) -c rw.c

clean:
	rm -f rw rw.o