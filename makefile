CC = gcc
LIBS = -lpng -lm
FLAGS = -O3 -Wall

all: sc

sc: sc.o image.o
	$(CC) sc.o image.o -o sc $(LIBS)

image.o: image.c image.h
	$(CC) $(FLAGS) -c $<

sc.o: sc.c image.h
	$(CC) -c $< $(FLAGS)

clean:
	rm -v *.o sc
