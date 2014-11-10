#!/bin/bash
cat << EOF
export TOOLCHAIN_${1}=
export CXXFLAGS_p_${1}="$(pkg-config --cflags --libs sdl2 | sed 's/ -lSDL2$//g') ${2}"
EOF
