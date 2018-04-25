static:
	make clear
	gcc -Wall -c library.c

	ar crs liblibrary.a library.o

	gcc -Wall main.c -o main -L. -llibrary

clear:
	rm -f *.o *.a *.so main
