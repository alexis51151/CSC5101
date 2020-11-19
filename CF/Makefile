FLAGS=-g
LDFLAGS=-lpthread


all: synchro

synchro: synchro.o
	$(CC) -o synchro synchro.o $(FLAGS) $(LDFLAGS)

synchro.o: synchro.c
	$(CC) -c synchro.c


clean:
	rm -f synchro synchro.o