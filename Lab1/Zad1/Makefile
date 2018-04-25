static:
	make clean
	gcc -c -Wall library.c
	ar rcs library.a library.o

shared:
	make clean
	gcc -c -Wall -fPIC library.c
	gcc -shared -fPIC -o library.so library.o

clean:
	rm -f *.o
