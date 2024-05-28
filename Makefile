main:main.o serial.o
	gcc main.o serial.o -o main
main.o: main.c
	gcc -c main.c -o main.o
serial.o:serial.c serial.h
	gcc -c serial.c -o serial.o