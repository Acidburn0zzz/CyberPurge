SOURCES=$(wildcard src/*.cpp)
SOURCES_gen=gen/gen.cpp
CXXFLAGS?=-lm -lc -lstdc++
CXXFLAGS_sdl=$(shell pkg-config --cflags --libs sdl2) -lSDL2_image -lSDL2_mixer
CXXFLAGS_my=-Iinclude -std=gnu++1y -lpthread -lrt -Wall
CXXFLAGS_p_debug=-O0 -ggdb -gdwarf-4
CXXFLAGS_p_release=-O3 -s -g0
.PHONY: all
all: bin/wip.debug bin/wip.release bin/gen.debug bin/gen.release

bin/wip.%: $(SOURCES)
	$(CC) -x c++ $(CXXFLAGS) $(CXXFLAGS_sdl) $(CXXFLAGS_my) $(CXXFLAGS_p_$*) -std=gnu++1y $(SOURCES) -o $@

bin/gen.%: $(SOURCES_gen)
	$(CC) -x c++ $(CXXFLAGS) $(CXXFLAGS_my) $(CXXFLAGS_p_$*) -std=gnu++1y $(SOURCES_gen) -o $@
