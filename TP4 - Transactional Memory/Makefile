all: tm

tm.o: 	tm.c
		gcc -c tm.c
tm:		tm.o
		gcc -o tm -g tm.o -lpthread


clean:
		rm -f tm.o tm

