FLAGS=-g
LDFLAGS=-lpthread


all: nvmm

nvmm:	nvmm.o
	gcc -o nvmm nvmm.o $(FLAGS) $(LDFLAGS)
nvmm.o:	nvmm.c nvmm.h
	gcc -c nvmm.c


clean:
	rm -f nvmm nvmm.o
