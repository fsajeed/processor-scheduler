-B: allocate.o 
	gcc -Wall -c allocate.c -lm
	gcc -Wall -o allocate allocate.o -lm

clean:
	rm -f *.o
