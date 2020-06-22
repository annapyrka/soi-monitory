CC = g++

all: main.o buffor.o 
	$(CC) main.o buffor.o -o monitor -pthread

main.o: main.cpp
	$(CC) -c main.cpp -o main.o -pthread

messagebuffer.o: buffor.cpp buffor.h monitor.h
	$(CC) -c buffor.cpp -o buffor.o -pthread

clean:
	rm -rf *.o monitor
