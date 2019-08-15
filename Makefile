COMPILER=clang++
CFLAGS=-Wno-c++17-extensions -lncurses -lm --debug -ggdb
EXE=mscpp
SOURCE=ms.cpp game.cpp

all: ms.o

ms.o: ms.cpp
	$(COMPILER) $(CFLAGS) $(SOURCE) -o $(EXE)

clean:
	rm $(EXE)
