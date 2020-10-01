BIN=main producer_consumer producer_consumers producer_consumers2
CFLAGS=
LDFLAGS=

all: $(BIN)

main: main.o
	$(CC) -o main main.o -lpthread

main.o : main.c
	$(CC) -c main.c

producer_consumer: producer_consumer.o
	$(CC) -o producer_consumer producer_consumer.o -lpthread

producer_consumer.o: producer_consumer.c
	$(CC) -c producer_consumer.c

producer_consumers: producer_consumers.o
	$(CC) -o producer_consumers producer_consumers.o -lpthread

producer_consumers.o: producer_consumers.c
	$(CC) -c producer_consumers.c

producer_consumers2: producer_consumers2.o
	$(CC) -o producer_consumers2 producer_consumers2.o -lpthread

producer_consumers2.o: producer_consumers2.c
	$(CC) -c producer_consumers2.c


clean:
	rm -f $(BIN)
