tests:
	make clean
	gcc -Wall main.c -o main
	echo "\nsys\n" >> wyniki.txt
	./main generate dane 1000 4 >> wyniki.txt
	./main copy dane kopia 1000 4 sys >> wyniki.txt
	./main sort dane 1000 4 sys >> wyniki.txt
	make clean_data
	./main generate dane 10000 4 >> wyniki.txt
	./main copy dane kopia 10000 4 sys >> wyniki.txt
	./main sort dane 10000 4 sys >> wyniki.txt
	make clean_data
	./main generate dane 5000 512 >> wyniki.txt
	./main copy dane kopia 5000 512 sys >> wyniki.txt
	./main sort dane 5000 512 sys >> wyniki.txt
	make clean_data
	./main generate dane 8000 512 >> wyniki.txt
	./main copy dane kopia 8000 512 sys >> wyniki.txt
	./main sort dane 8000 512 sys >> wyniki.txt
	make clean_data
	./main generate dane 2000 4096 >> wyniki.txt
	./main copy dane kopia 2000 4096 sys >> wyniki.txt
	./main sort dane 2000 4096 sys >> wyniki.txt
	make clean_data
	./main generate dane 2600 4096 >> wyniki.txt
	./main copy dane kopia 2600 4096 sys >> wyniki.txt
	./main sort dane 2600 4096 sys >> wyniki.txt
	make clean_data
	./main generate dane 1200 8192 >> wyniki.txt
	./main copy dane kopia 1200 8192 sys >> wyniki.txt
	./main sort dane 1200 8192 sys >> wyniki.txt
	make clean_data
	./main generate dane 1600 8192 >> wyniki.txt
	./main copy dane kopia 1600 8192 sys >> wyniki.txt
	./main sort dane 1600 8192 sys >> wyniki.txt
	echo "\nlib\n" >> wyniki.txt
	./main generate dane 1000 4 >> wyniki.txt
	./main copy dane kopia 1000 4 lib >> wyniki.txt
	./main sort dane 1000 4 lib >> wyniki.txt
	make clean_data
	./main generate dane 10000 4 >> wyniki.txt
	./main copy dane kopia 10000 4 lib >> wyniki.txt
	./main sort dane 10000 4 lib >> wyniki.txt
	make clean_data
	./main generate dane 5000 512 >> wyniki.txt
	./main copy dane kopia 5000 512 lib >> wyniki.txt
	./main sort dane 5000 512 lib >> wyniki.txt
	make clean_data
	./main generate dane 8000 512 >> wyniki.txt
	./main copy dane kopia 8000 512 lib >> wyniki.txt
	./main sort dane 8000 512 lib >> wyniki.txt
	make clean_data
	./main generate dane 2000 4096 >> wyniki.txt
	./main copy dane kopia 2000 4096 lib >> wyniki.txt
	./main sort dane 2000 4096 lib >> wyniki.txt
	make clean_data
	./main generate dane 2600 4096 >> wyniki.txt
	./main copy dane kopia 2600 4096 lib >> wyniki.txt
	./main sort dane 2600 4096 lib >> wyniki.txt
	make clean_data
	./main generate dane 1200 8192 >> wyniki.txt
	./main copy dane kopia 1200 8192 lib >> wyniki.txt
	./main sort dane 1200 8192 lib >> wyniki.txt
	make clean_data
	./main generate dane 1600 8192 >> wyniki.txt
	./main copy dane kopia 1600 8192 lib >> wyniki.txt
	./main sort dane 1600 8192 lib >> wyniki.txt
clean:
	rm -f *.o main dane kopia wyniki.txt
clean_data:
	rm dane kopia
