compiler = g++
version = c++11

huffman: huffman.o
	$(compiler) -std=$(version) huffman.o -o huffman

huffman.o: src/huffman.cpp
	$(compiler) -std=$(version) -c src/huffman.cpp

src/huffman.cpp:
	echo "Please re-clone the repo at git@github.com:AbdallaMourad/huffman-compression-tool.git"

clean:
	rm -f $(wildcard *.o) huffman