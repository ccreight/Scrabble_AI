#include "Player.h"
#include <string>
#include <iostream>

using namespace std;

Player::Player(std::string const & name, size_t maxTiles){

	_name = name;
	_maxTiles = maxTiles;
	_points = 0;
	firstMove = false;
	isCPUS = false;
	isCPUL = false;

}

Player::~Player(){

	std::set<Tile*>::iterator iter = hand.begin();

	while(iter != hand.end()){ //deletes the player's tiles

		std::set<Tile*>::iterator holder = iter;
		delete *iter;
		iter = ++holder;

	}

}

std::set<Tile*> Player::getHandTiles() const{

	return hand;

}

bool Player::hasTiles(std::string const & move, bool resolveBlanks) const{

	std::string holder = move;
	unsigned int count = 0;
	std::vector<char> handLetters;
	int blankTiles = 0;

	//creates a vector of all the player's hand tiles' letters
	for(std::set<Tile*>::iterator iter = hand.begin(); 
		iter != hand.end(); ++iter){

		handLetters.push_back((*iter)->getLetter());

	}

	//checks for equivalency / containment
	for(unsigned int x = 0; x < holder.length(); x++){

		for(unsigned int y = 0; y < handLetters.size(); y++){

			if(handLetters[y] == holder[x]){

				handLetters[y] = '_'; //marks that letter as used
				count++;

				//accounts for blanks
				if(holder[x] == '?' && resolveBlanks){
					x++;
					blankTiles++;
				}

				break;

			}

		}

	}

	if(count != move.length() - blankTiles){

		return false;

	}

	return true;

}

std::vector<Tile*> Player::takeTiles(std::string const & move, 
	bool resolveBlanks){

	std::string temp = move;
	std::vector<Tile*> returned;

	for(unsigned int x = 0; x < temp.length(); x++){ //traverses the move string

		//traverses the player's hand
		for(std::set<Tile*>::iterator iter = hand.begin(); 
			iter != hand.end(); ++iter){

			Tile* holder = (*iter);

			if(holder->getLetter() == temp[x]){ //if it's the right letter

				if(holder->getLetter() == '?' && resolveBlanks){
					holder->useAs(temp[x+1]); //assigns letter to blank
					x++;
				}

				hand.erase(iter);
				returned.push_back(*iter);
				break;

			}

		}

	}

	return returned;

}

void Player::addTiles(std::vector<Tile*> const & tilesToAdd){

	//traverses vector, pushing tile pointers onto hand
	for(unsigned int x = 0; x < tilesToAdd.size(); x++){
		hand.insert(tilesToAdd[x]);
	}

}