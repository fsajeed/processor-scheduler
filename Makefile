-B: allocate.o 
	gcc -Wall -c allocate.c
	gcc -Wall -o allocate allocate.o 

clean:
	rm -f *.o
