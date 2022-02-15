## ifeq ($@, nes)
#	CFLAGS:="-I/Users/clentz/Downloads/MacOSX10.9.sdk/usr/include -I/Users/clentz/Library/Developer/CommandLineTools/usr/include -Iinclude"
#	LDFLAGS:="-L/Users/clentz/Downloads/MacOSX10.9.sdk/usr/lib/ -lSystem -L/Users/clentz/Downloads/MacOSX10.9.sdk/usr/lib/System -Wl,-no_pie"
## endif
SRC_DIR := /Users/clentz/bin/cpp/nes/6502/src
OBJ_DIR := /Users/clentz/bin/cpp/nes/6502/obj
BIN_DIR := /Users/clentz/bin/cpp/nes/6502/.

EXE := nes

SRC := $(SRC_DIR)/nes.cpp:$(SRC_DIR)/libbase.cpp
OBJ := $($(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)



all: $(EXE)

prg: prg.o
	ld65 --config nes.cfg -o prg.nes prg.o
	rm prg.o
prg.o:
	ca65 --cpu 6502 -o prg.o prg.s

nes: $(OBJ_DIR) $(OBJ_DIR)/nes.o
# 	echo clang++ $(LDFLAGS) $(OBJ) -o $@
	c++ $(LDFLAGS) $(OBJ_DIR)/nes.o -o nes -Wunused-command-line-argument
	$(make clean)

$(OBJ_DIR)/nes.o: $(OBJ_DIR)/libbase.o
	c++ -c $(OBJ_DIR)/libbase.o $(SRC_DIR)/nes.cpp -o $(OBJ_DIR)/nes.o -Wunused-command-line-argument

$(OBJ_DIR)/libbase.o:
	c++ -Wall -c $(SRC_DIR)/libbase.cpp -d obj -o $(OBJ_DIR)/libbase.o

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(OBJ_DIR)

.PHONY: all clean

#-include $(OBJ:.o=.d )


#
#nes.cpp:
# 	clang++ ${CXXFLAGS} ${LDFLAGS} -c libbase.cpp -o libbase.o
# 	clang++ ${CXXFLAGS} ${LDFLAGS} -o nes -Wexpansion-to-defined -v nes.cpp
# lib:
# 	clang++ ${CXXFLAGS} ${LDFLAGS} -c libbase.cpp -o libbase.o
#
# nesonly:
# 	clang++ ${CXXFLAGS} ${LDFLAGS} -o nes -Wexpansion-to-defined -v nes.cpp