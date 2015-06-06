CXX=g++
CXXFLAGS=-O3 -Wall -Wextra

addnoise: addnoise_function.o io_png.o mt19937ar.o main.o
	$(CXX) $(CXXFLAGS) $^ -o $@ -lm -lpng -lfftw3f -lfftw3f_threads -fopenmp

ipol: addnoise
	mv $< ../../bin/

clean:
	-rm -rf *.o addnoise
