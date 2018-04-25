static:
	make clean
	gcc -Wall -c library.c

	ar crs liblibrary.a library.o

	gcc -Wall main.c -o main -L. -llibrary

shared:
	make clean
	gcc -c -Wall -fPIC library.c
	gcc -shared -fPIC -o library.so library.o

	gcc -Wall main.c -o main -L. -llibrary

clean:
	rm -f *.o main
