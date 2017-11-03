CXX = g++

SDL_LIB = -L/usr/local/lib -lSDL2 -lSDL2_image -lSDL2_mixer -Wl,-rpath=/usr/local/lib
SDL_INCLUDE = -I/usr/local/include

CXXFLAGS = -Wall -c -std=c++0x $(SDL_INCLUDE)
LDFLAGS = $(SDL_LIB)
EXE = Game 

all: $(EXE)

$(EXE): main.o events.o img_pcx.o primitives.o
	$(CXX) main.o events.o img_pcx.o primitives.o $(LDFLAGS) -o $@

main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< -o $@

events.o: events.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< -o $@

img_pcx.o: img_pcx.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< -o $@

primitives.o: primitives.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< -o $@

clean:
	rm *.o && rm $(EXE)
