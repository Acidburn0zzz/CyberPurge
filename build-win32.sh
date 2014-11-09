#!/bin/bash
dir=${2}-w64-mingw32
cat << EOF
export TOOLCHAIN_${1}=${dir}-
export CXXFLAGS_p_${1}="-Idl/win32/${dir}/include/SDL2 -Ldl/win32/${dir}/lib -mconsole"
export LIBS_p_${1}="-lmingw32 -lSDL2main -mwindows"
EOF
rm -rf files/${1}
mkdir -p files/${1}
cp -r /usr/${dir}/sys-root/mingw/bin/*.dll dl/win32/${dir}/bin/*.dll files/${1}/
