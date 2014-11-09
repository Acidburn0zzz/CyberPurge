#!/bin/bash
#	./build-unix linux-x86 i686
export CXXFLAGS="-g0 -O3 -s"
(
	echo export BINSUFFIX=
	./build-unix.sh linux_x86_64 x86_64
	echo make bin/linux_x86_64/.done -j4 --keep-going
) | bash
(
	echo export BINSUFFIX=.exe
	./build-win32.sh win32 i686
	./build-win32.sh win64 x86_64
	echo make bin/{win32,win64}/.done -j4
) | bash
exit
