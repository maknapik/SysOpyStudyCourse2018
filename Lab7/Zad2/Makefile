all:
	make clean
	gcc -Wall client.c -o client -lrt -pthread
	gcc -Wall server.c -o server -lrt -pthread
clean:
	rm -f *.o client server
