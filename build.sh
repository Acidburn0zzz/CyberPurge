#!/bin/bash
(
	echo export BINSUFFIX=
	./build-unix.sh release "-O3 -g0 -s"
	echo make bin/release/.done -j4 --keep-going
) | bash
