all:
	gcc -Wall -o main main.c -lpthread -lm
	./main 1 input1 filter3 output1 >> Times.txt
	./main 2 input1 filter3 output1 >> Times.txt
	./main 4 input1 filter3 output1 >> Times.txt
	./main 8 input1 filter3 output1 >> Times.txt
	./main 1 input1 filter25 output1 >> Times.txt
	./main 2 input1 filter25 output1 >> Times.txt
	./main 4 input1 filter25 output1 >> Times.txt
	./main 8 input1 filter25 output1 >> Times.txt
	./main 1 input1 filter65 output1 >> Times.txt
	./main 2 input1 filter65 output1 >> Times.txt
	./main 4 input1 filter65 output1 >> Times.txt
	./main 8 input1 filter65 output1 >> Times.txt
	./main 1 input2 filter3 output2 >> Times.txt
	./main 2 input2 filter3 output2 >> Times.txt
	./main 4 input2 filter3 output2 >> Times.txt
	./main 8 input2 filter3 output2 >> Times.txt
	./main 1 input2 filter25 output2 >> Times.txt
	./main 2 input2 filter25 output2 >> Times.txt
	./main 4 input2 filter25 output2 >> Times.txt
	./main 8 input2 filter25 output2 >> Times.txt
	./main 1 input2 filter65 output2 >> Times.txt
	./main 2 input2 filter65 output2 >> Times.txt
	./main 4 input2 filter65 output2 >> Times.txt
	./main 8 input2 filter65 output2 >> Times.txt
clean:
	rm main maina
