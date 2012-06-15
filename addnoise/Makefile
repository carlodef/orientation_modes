CFLAGS=-O -Wall -Wextra

all: addnoise_function.o io_png.o mt19937ar.o main.o 
	g++ -O -Wall -Wextra -o addnoise *.o -lpng -lfftw3f -lm -lfftw3f_threads -fopenmp

ipol: addnoise_function.o io_png.o mt19937ar.o main.o 
	g++ -O -Wall -Wextra -o ../../bin/addnoise *.o -lpng -lfftw3f -lm -lfftw3f_threads -fopenmp

clean: 
	rm -rf *.o addnoise

