#!/bin/bash
dir=${2}-redhat-linux
cat << EOF
export TOOLCHAIN_${1}="${dir}-"
export CXXFLAGS_p_${1}="$(${dir}-gnu-pkg-config --cflags --libs sdl2 | sed 's/ -lSDL2$//g')"
EOF
