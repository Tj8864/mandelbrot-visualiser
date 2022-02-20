all:
	g++ -Isrc/include -Lsrc/lib -o main main.cpp -lmingw64 -lSDL2main -lSDL2