CPPFLAGS = g++ -Wall -g -std=c++11
FILES = Dictionary.cpp Board.cpp Bag.cpp ConsolePrinter.cpp Exceptions.cpp Move.cpp Player.cpp rang.h Tile.h Square.h

all: scrabble trie
	-@echo "--- All Built!---"

scrabble: scrabble.cpp Board.h Move.h Dictionary.h Bag.h ConsolePrinter.h Exceptions.h Player.h cpus.h cpul.h Trie.h
	$(CPPFLAGS) rang.h Tile.h Square.h $^ -o scrabble

trie: Trie.h Trie.cpp
	$(CPPFLAGS) Trie.h Trie.cpp -o trie