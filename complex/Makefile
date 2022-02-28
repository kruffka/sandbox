CC = gcc
CFLAGS = -Wall
SRC = main.c functions.c
TARGET = main

.PHONY: clean

all: $(TARGET)

main.o: main.c
			gcc -c -o main.o main.c
functions.o: functions.c
			gcc -c -o functions.o functions.c

$(TARGET): main.o functions.o
			gcc -o $(TARGET) main.o functions.o

clean:
	rm $(TARGET) *.o