all: generate run

generate: generate.c
	gcc -o generate generate.c

linearbarrier.o: linearbarrier.c 
	gcc -c -g -lm linearbarrier.c

heap.o: heap.c
	gcc -c -g -lm heap.c

pqsort.o: pqsort.c linearbarrier.o heap.o
	gcc -c -g -lm -lpthread pqsort.c

run: driver.c pqsort.o 
	gcc -o  run driver.c pqsort.o linearbarrier.o heap.o -lm -lpthread

clean:
	rm generate pqsort.o run linearbarrier.o heap.o
