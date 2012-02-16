# Variables
CXX = g++
CXXFLAGS = -W -Wall -Wno-write-strings

# all

alll: obj all

all: obj/main.o obj/modes_detection.o obj/Histo.o obj/libpng_io.o 
	$(CXX) $^ -lpng -o modes_detection $(CXXFLAGS)

obj:
	mkdir obj

obj/%.o: src/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)


