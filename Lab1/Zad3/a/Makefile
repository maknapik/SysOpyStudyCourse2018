static:
	make clean
	gcc -Wall -c library.c
	ar crs liblibrary.a library.o
	gcc -Wall main.c -o main -L. -llibrary
	echo "\nstatic library\n" >> results3a.txt
	make tests

shared:
	make clean
	gcc -c -Wall -fPIC library.c
	gcc -shared -fPIC -o library.so library.o
	gcc -Wall main.c -o main -L. -llibrary
	echo "\nshared library\n" >> results3a.txt
	make tests

dll:
	make clean
	gcc -c -Wall -fPIC library.c
	gcc -shared -fPIC -o library.so library.o
	gcc -Wall main.c -o main -L. -ldl -D DLL
	echo "\ndll library\n" >> results3a.txt
	make tests

clean:
	rm -f *.o main

tests:
	# static
	echo "\nstatic\n" >> results3a.txt
	./main 500000 10 static c f 50 ad 1000 adc 1000 >> results3a.txt
	./main 500000 100 static c f 50 ad 1000 adc 1000 >> results3a.txt
	./main 500000 500 static c f 50 ad 1000 adc 1000 >> results3a.txt
	./main 500000 1000 static c f 50 ad 1000 adc 1000 >> results3a.txt
	
	# dynamic
	echo "\ndynamic\n" >> results3a.txt
	./main 500000 10 dynamic c f 50 ad 1000 adc 1000 >> results3a.txt
	./main 500000 100 dynamic c f 50 ad 1000 adc 1000 >> results3a.txt
	./main 500000 500 dynamic c f 50 ad 1000 adc 1000 >> results3a.txt
	./main 500000 1000 dynamic c f 50 ad 1000 adc 1000 >> results3a.txt
