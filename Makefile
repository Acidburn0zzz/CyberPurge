CONFIG_BINARY_SUFFIX?=
SOURCES=$(wildcard src/*.cpp)
SOURCES_gen=gen/gen.cpp
LIBS:=-lSDL2 -lSDL2_image -lSDL2_mixer
CXXFLAGS_my:=-Iinclude -std=gnu++1y -Wall
.PHONY: all assets bin/%

bin/%/wip$(BINSUFFIX): $(SOURCES)
	@mkdir -p bin/$*
	$(TOOLCHAIN_$*)g++ -x c++ $(CXXFLAGS) $(CXXFLAGS_my) $(CXXFLAGS_p_$*) $(CXXFLAGS_sdl_$p) -std=gnu++1y $(SOURCES) -o $@ $(LIBS_p_$*) $(LIBS)

bin/%/gen$(BINSUFFIX): $(SOURCES_gen)
	@mkdir -p bin/$*
	$(TOOLCHAIN_$*)g++ -x c++ $(CXXFLAGS) $(CXXFLAGS_my) $(CXXFLAGS_p_$*) -std=gnu++1y $(SOURCES_gen) -o $@

bin/%/.done: bin/%/gen$(BINSUFFIX) bin/%/wip$(BINSUFFIX)
	@touch $@

assets: ass/init.ogg ass/soundtrack.ogg ass/enemy-0-d.ogg ass/enemy-1-d.ogg ass/enemy-2-d.ogg ass/w-sg.ogg ass/w-bb.ogg ass/endgame.ogg
ass/%.ogg: ass/%.mp3
	ffmpeg -y -i $< -codec:a libvorbis -q:a 2 $@

.SECONDARY:
