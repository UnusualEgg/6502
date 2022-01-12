CXXFLAGS:=-I/Users/clentz/Downloads/MacOSX10.9.sdk/usr/include -I/Users/clentz/Library/Developer/CommandLineTools/usr/include -Iinclude
LDFLAGS:=-L/Users/clentz/Downloads/MacOSX10.9.sdk/usr/lib/ -lSystem -L/Users/clentz/Downloads/MacOSX10.9.sdk/usr/lib/System -Wl,-no_pie

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := .

EXE := $(BIN_DIR)/nes

SRC := $(SRC_DIR)/nes.cpp:$(SRC_DIR)/libbase.cpp
OBJ := $($(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)


all: $(EXE)

$(EXE): $(OBJ_DIR) $(OBJ_DIR)/nes.o
# 	echo clang++ $(LDFLAGS) $(OBJ) -o $@
	clang++ $(LDFLAGS) $(OBJ_DIR)/nes.o -o nes
	$(make clean)

$(OBJ_DIR)/nes.o: $(OBJ_DIR)/libbase.o
	clang++ $(CXXFLAGS) $(CFLAGS) -c $(OBJ_DIR)/libbase.o $(SRC_DIR)/nes.cpp -o $(OBJ_DIR)/nes.o

$(OBJ_DIR)/libbase.o:
	clang++ $(CXXFLAGS) $(LDFLAGS) -c $(SRC_DIR)/libbase.cpp -d obj -o $(OBJ_DIR)/libbase.o

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(OBJ_DIR)

.PHONY: all clean

-include $(OBJ:.o=.d)

# nes: nes.cpp
# 	clang++ ${CXXFLAGS} ${LDFLAGS} -c libbase.cpp -o libbase.o
# 	clang++ ${CXXFLAGS} ${LDFLAGS} -o nes -Wexpansion-to-defined -v nes.cpp
# lib:
# 	clang++ ${CXXFLAGS} ${LDFLAGS} -c libbase.cpp -o libbase.o

# nesonly:
# 	clang++ ${CXXFLAGS} ${LDFLAGS} -o nes -Wexpansion-to-defined -v nes.cpp