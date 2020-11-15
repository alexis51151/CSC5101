
all: queue

queue: queue.o
	$(CC) queue.o -o queue -lpthread

queue.o: queue.c
	$(CC) -c queue.c

clean:
	rm -f *.o