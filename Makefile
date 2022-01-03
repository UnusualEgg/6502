CFLAGS:=-I/Users/clentz/Downloads/MacOSX10.9.sdk/usr/include -I/Users/clentz/Library/Developer/CommandLineTools/usr/include
LDFLAGS:=-L/Users/clentz/Downloads/MacOSX10.9.sdk/usr/lib/ -lSystem -L/Users/clentz/Downloads/MacOSX10.9.sdk/usr/lib/System -Wl,-no_pie

nes: nes.cpp
	clang++ ${CFLAGS} ${LDFLAGS} -o nes -Wexpansion-to-defined nes.cpp