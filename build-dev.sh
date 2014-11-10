#!/bin/bash
(
	echo export BINSUFFIX=
	./build-unix.sh debug "-O0 -g4 -gdwarf-4"
	./build-unix.sh release "-O3 -g0 -s"
	echo make bin/{debug,release}/.done -j4 --keep-going
) | bash
(
	echo export BINSUFFIX=.exe
	./build-win32.sh win32 "-O3 -g0 -s" i686
	./build-win32.sh win64 "-O3 -g0 -s" x86_64
	echo make bin/{win32,win64}/.done -j4
) | bash
