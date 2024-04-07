CC = gcc
LIBS = -lpng -lm
FLAGS = -O3 -Wall

all: car

car: car.o image.o
	$(CC) car.o image.o -o car $(LIBS)

image.o: image.c image.h
	$(CC) $(FLAGS) -c $<

car.o: car.c image.h
	$(CC) -c $< $(FLAGS)

clean:
	rm -v *.o car
