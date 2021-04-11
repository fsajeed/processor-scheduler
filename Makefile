-B: allocate.o functions.o
	gcc -Wall -c allocate.c -lm
	gcc -Wall -c functions.c -lm
	gcc -Wall -o allocate allocate.o functions.o -lm

clean:
	rm -f *.o
