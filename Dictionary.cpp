#include "Dictionary.h"
#include <string>
#include <fstream>
#include "Exceptions.h"

Dictionary::Dictionary(std::string dictionary_file_name){

	std::ifstream dictionaryFileStream(dictionary_file_name);

	if(dictionaryFileStream.fail()){

		throw FileException("DICTIONARY");

	}

	std::string line;

	//reads in each word
	while(getline(dictionaryFileStream, line)){

		wordBank.insert(line); //pushes it into the wordbank

	}

}

Dictionary::~Dictionary(){

	
	
}

bool Dictionary::isLegalWord(std::string const & word){

	std::string check = word;
	TrieNode* end = wordBank.prefix(check);

	if(end == nullptr){
		return false;
	}

	return end->inSet;

}