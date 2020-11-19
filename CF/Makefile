FLAGS=-g
LDFLAGS=-lpthread


all: synchro rcl

synchro: synchro.o
	$(CC) -o synchro synchro.o $(FLAGS) $(LDFLAGS)

synchro.o: synchro.c
	$(CC) -c synchro.c

rcl: rcl.o
	$(CC) -o rcl rcl.o $(FLAGS) $(LDFLAGS)

rcl.o: rcl.c
	$(CC) -c rcl.c



clean:
	rm -f synchro rcl *.o