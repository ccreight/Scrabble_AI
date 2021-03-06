#include "cpul.h"
#include <algorithm>
#include <string>

CPUL::CPUL(std::string const & name, size_t maxTiles):Player(name, maxTiles){
	isCPUS = false;
	isCPUL = true;
}

/*
	Gets valid words from given characters in letters vector
*/
void CPUL::getWords(vector<char> letters, 
	std::set<std::string>& words, Dictionary& dict, char onBoard){

	std::vector<std::string> permutations; //holds all permutations of letters

	while(next_permutation(letters.begin(), letters.end())){ //gets perms
		std::string str;
		for(size_t x = 0; x < letters.size(); x++){
			str += letters[x];
		}
		permutations.push_back(str);
	}

	//if the permutation of letters is a valid word, add it to the set
	for(size_t x = 0; x < permutations.size(); x++){
		for(size_t y = 0; y < letters.size()-1; y++){
			if(dict.isLegalWord(permutations[x].substr(y,letters.size()))
				&& (onBoard == '.' || 
					permutations[x].find(onBoard) != std::string::npos)){

				words.insert(permutations[x].substr(y,letters.size()));
			}
		}
	}
}

/*
	Gets the list of valid words from the hand
	If there are blanks, it runs all possible additional letters
	and checks if there are possible words formed
*/
std::vector<std::string> CPUL::playWord(std::set<Tile*> hand, 
	Square* square, int x, int y, Dictionary& dict){

	std::vector<char> letters;
	std::set<std::string> words;
	char onBoard = '.';

	if(square != nullptr){ //not starting move
		onBoard = square->getLetter();
		letters.push_back(onBoard);
	}

	int blanks = 0;

	//pushes letters in hand into vector
	for(std::set<Tile*>::iterator it = hand.begin(); it != hand.end(); ++it){
		if((*it)->getLetter() < 123 && (*it)->getLetter() > 96){
			letters.push_back((*it)->getLetter());
		}
		else{
			blanks++;
		}
	}

	getWords(letters, words, dict, onBoard); //gets legal words

	if(blanks == 1){ //adds new possible words
		for(int y = 0; y < 26; y++){
			letters.push_back(97+y);
			getWords(letters,words,dict,onBoard);
			letters.pop_back();
		}
	}

	if(blanks == 2){ //adds new possible words
		for(int x = 0; x < 26; x++){
			for(int y = 0; y < 26; y++){
				letters.push_back(97+x);
				letters.push_back(97+y);
				getWords(letters,words,dict,onBoard);
				letters.pop_back();
				letters.pop_back();
			}
		}
	}

	std::vector<std::string> vec;

	//creates the vector to be returned
	for(std::set<std::string>::iterator it = words.begin(); it != words.end();
		++it){
		vec.push_back(*it);
	}

	return vec;

}

/*
	Based on what valid words can be played, gets the best option and plays it
*/
void CPUL::findBestMove(Bag* bag, Board* board, Dictionary& dict){

	std::pair<int, std::string> move;
	std::map<int, std::vector<std::string>> words;
	std::set<int> lengths;
	std::vector<int> sortedLengths;
	int maxLength = 0;
	PlaceMove p = PlaceMove(0, 0, true, "zxz", this);
	std::string maxString = "";

	int blanks = 0;

	//accounts for blanks
	for(std::set<Tile*>::iterator it = hand.begin(); it != hand.end(); ++it){
		if((*it)->getLetter() == '?'){
			blanks++;
		}
	}

	//contains the letters in the hand - to be used for accounting for blanks
	std::set<char> handLetters;
	for(std::set<Tile*>::iterator it = hand.begin(); it != hand.end(); ++it){
		handLetters.insert((*it)->getLetter());
	}

	bool gotWords = false; //used to account for if it's the first move

	for(size_t x = 1; x <= board->getRows(); x++){
		for(size_t y = 1; y <= board->getColumns(); y++){
			//creates the strings to actually be played
			//so moves don't include letter already on the board
			if(getFirstMove() || board->getSquare(x,y)->isOccupied()){
				std::vector<std::string> holder;
				holder = playWord(hand, board->getSquare(x,y), x, y, dict);

				//adds the string to holder, puts into map based on length
				for(size_t z = 0; z < holder.size(); z++){
					if(blanks == 1){ //accounts for one blank
						int index = -1;
						for(size_t i = 0; i < holder[z].length(); i++){
							if(handLetters.find(holder[z][i]) == 
								handLetters.end()){
								index = i;
							}
						}
						if(index != -1 && index < (int)(holder[z].length())){
							holder[z] = holder[z].substr(0,index) + '?' + 
								holder[z].substr(index);
						}
					}
					if(blanks == 2){ //accounts for two blanks
						int index1 = -1, index2 = -1;
						for(size_t i = 0; i < holder[z].length(); i++){
							if(handLetters.find(holder[z][i]) == 
								handLetters.end() && index1 != -1){
								index1 = i;
							}
							else if(handLetters.find(holder[z][i]) == 
								handLetters.end()){
								index2 = i;
							}
						}
						if(index1 != -1 && index1 < (int)holder[z].length() 
							&& index2 != -1 && 
							index2 < (int)holder[z].length()){
							holder[z] = holder[z].substr(0,index1) + '?' + 
							holder[z].substr(index1, index2-index1) + '?' + 
							holder[z].substr(index2);
						}
					}

					std::string str;
					size_t index = 0;
					if(board->getSquare(x,y)->isOccupied()){
						index = holder[z].find(board->getSquare(x,y)->getLetter());
						str = holder[z].substr(0,index) + 
						holder[z].substr(index+1);
						PlaceMove m1 = PlaceMove(x-index, y, true, str, this);
						PlaceMove m2 = PlaceMove(x, y-index, false, str, this);

						if(m1.tryPlace(*board, *bag, dict)){
							int x = str.length();
							// words[x].push_back(str);
							if(x > maxLength){
								maxLength = x;
								maxString = str;
								p = m1;
							}
						}
						if(m2.tryPlace(*board, *bag, dict)){
							int x = str.length();
							// words[x].push_back(str);
							if(x > maxLength){
								maxLength = x;
								maxString = str;
								p = m2;
							}
						}
					}
					else if(getFirstMove()){
						str = holder[z];
						words[str.length()-blanks+1].push_back(str);
						lengths.insert(str.length()-blanks+1);
					}
				}
			}
			if(getFirstMove()){ //if the first move
				gotWords = true;
				break;
			}
		}
		if(gotWords){ //if the first move
			break;
		}
	}

	if(p.tryPlace(*board, *bag, dict)){
		p.execute(*board, *bag, dict);
		return;
	}
	
	//holds the lengths of all words generated from least to greatest
	for(std::set<int>::iterator it = lengths.begin(); it != lengths.end(); 
		++it){
		sortedLengths.push_back(*it);
	}
	sort(sortedLengths.begin(), sortedLengths.end());

	//used to account for issues that arose with the vector
	size_t start = 0;
	for(size_t x = 0; x < sortedLengths.size(); x++){
		if(sortedLengths[x] < 0){
			start++;
		}
	}

	//if AI ends up needing to exchange tiles
	std::string handForExchange = "";
	for(std::set<Tile*>::iterator it = hand.begin(); it != hand.end(); ++it){
		handForExchange += (*it)->getLetter();
	}

	if(getFirstMove()){ //accounts for if it's the first move of game
		for(size_t i = sortedLengths.size()-1; i >= start; i--){

			for(size_t z = 0; z < words[sortedLengths[i]].size(); z++){
				std::cout << words[sortedLengths[i]][z] << " ";
				PlaceMove m1 = PlaceMove(board->getStartX(), board->getStartY(), 
					true, words[sortedLengths[i]][z], this);
				if(m1.tryPlace(*board, *bag, dict)){
					m1.execute(*board, *bag, dict);
					setFirstMove(false);
					return;
				}
			}
		}
	}

	ExchangeMove m = ExchangeMove(handForExchange, this);
	m.execute(*board, *bag, dict);
}