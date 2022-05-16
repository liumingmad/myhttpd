obj = main.c

all : ${obj} 
	cc -g -o main ${obj}

run : main
	./main

clean :
	-rm main
