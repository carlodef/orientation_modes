
CFLAGS=-O -Wall -Wextra

all: addnoise_function.o io_png.o mt19937ar.o main.o 
	g++ -O -Wall -Wextra -lpng -lfftw3f -lm -lfftw3f_threads -fopenmp *.o -o addnoise

clean: 
	rm -rf *.o addnoise

