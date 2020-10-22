FLAGS=-g -march=native
LDFLAGS=-lpthread


all: nvmm

nvmm:	nvmm.o
	gcc -o nvmm nvmm.o $(LDFLAGS)
nvmm.o:	nvmm.c nvmm.h
	gcc -c nvmm.c $(FLAGS)


clean:
	rm -f nvmm nvmm.o nvmm.dat out.dat
