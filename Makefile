CXX = g++

SDL_LIB = -L/usr/local/lib -lSDL2 -lSDL2_image -lSDL2_mixer -Wl,-rpath=/usr/local/lib
SDL_INCLUDE = -I/usr/local/include

CXXFLAGS = -Wall -c -std=c++0x $(SDL_INCLUDE)
LDFLAGS = $(SDL_LIB)
EXE = Game 

all: $(EXE)

$(EXE): main.o 
	$(CXX) main.o $(LDFLAGS) -o $@

main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< -o $@



clean:
	rm *.o && rm $(EXE)
