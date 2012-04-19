# variables
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror -O3

# compilation 
all:
	cd src; $(CXX) main.cpp Histo.cpp modes_detection.cpp libpng_io.cpp -lpng -o ../modes_detection $(CXXFLAGS)
ipol:
	cd src; $(CXX) main.cpp Histo.cpp modes_detection.cpp libpng_io.cpp -lpng -o ../../../bin/modes_detection $(CXXFLAGS)
	
