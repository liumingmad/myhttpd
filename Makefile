obj = main.c mq.c utils.c wrap.c

all : ${obj} 
	cc -g -o main ${obj} -lpthread

run : main
	./main

clean :
	-rm main
