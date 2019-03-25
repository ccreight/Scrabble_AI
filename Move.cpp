#include "Move.h"
#include <sstream>
#include <string>
#include <iostream>

using namespace std;

Move* Move::parseMove(std::string moveString, Player &p){

	std::string str = moveString;
	std::stringstream ss(str);

	std::string move;
	ss >> move; //reads in the type of move

	//sets all letters to lowercase
	for(unsigned int x = 0; x < move.length(); x++){
		move[x] = std::tolower(move[x]);
	}

	Move* decision;

	if(move == "pass"){

		decision = new PassMove(&p);

	}
	else if(move == "exchange"){

		//reads in necessary values for an exchange move
		std::string tiles;
		ss >> tiles;

		if(ss.fail()){
			return nullptr;
		}

		//sets all tiles to lowercase
		for(unsigned int x = 0; x < tiles.length(); x++){
			tiles[x] = std::tolower(tiles[x]);
		}

		decision = new ExchangeMove(tiles, &p);

	}
	else if(move == "place"){

		int x, y;
		char alignment;
		std::string tiles;

		//reads in necessary values for a place move
		ss >> alignment;
		ss >> x >> y;
		ss >> tiles;

		if(ss.fail()){
			return nullptr;
		}

		//sends all letters to lowercase
		for(unsigned int x = 0; x < tiles.length(); x++){
			tiles[x] = std::tolower(tiles[x]);
		}

		//accounts for the orientation
		if(alignment == '-'){
			decision = new PlaceMove(x, y, true, tiles, &p);
		}
		else if(alignment == '|'){
			decision = new PlaceMove(x, y, false, tiles, &p);
		}
		else{
			return nullptr;
		}

	}

	else{

		return nullptr;

	}

	return decision;

}

Move::Move(Player* player){

	_player = player;

}

Move::~Move(){

	//need to put stuff here?

}

PassMove::PassMove(Player* player):Move(player){

	passing = true;

}

bool PassMove::execute(Board & board, Bag & bag, Dictionary & dictionary){

	return true;

}

ExchangeMove::ExchangeMove(std::string tileString, Player * p):Move(p){

	passing = false;
	tiles = tileString;

}

bool ExchangeMove::execute(Board & board, Bag & bag, Dictionary & dictionary){

	if(!_player->hasTiles(tiles, false)){ //player doesn't have tiles
		cout << 
		"\033[3;94mError: You don't have those tiles - please try again\033[0m"
		 << endl;
		return false;
	}

	//puts player's tiles back in bag, adds new tiles to player's hand
	std::vector<Tile*> exchanged;
	exchanged = _player->takeTiles(tiles, false);
	bag.addTiles(exchanged);
	_player->addTiles(bag.drawTiles(tiles.length()));

	return true;

}

PlaceMove::PlaceMove(size_t x, size_t y, bool horizontal, 
	std::string tileString, Player * p):Move(p){

	passing = false;
	tiles = tileString;
	_x = x;
	_y = y;
	h = horizontal;

}

bool PlaceMove::execute(Board & board, Bag & bag, Dictionary & dictionary){

	int doubled = 0; //times they got a double word multiplier
	int tripled = 0; //times they got a triple word multiplier

	int wordPoints = 0;
	int letterPoints = 0; //used to account for multipliers

	int index = 0, actualLength;
	int blankTiles = 0; //used when drawing tiles back at the end of move

	for(size_t x = 0; x < tiles.length(); x++){ //counts blanks

		if(tiles[x] == '?'){
			blankTiles++;
		}

	}
	actualLength = tiles.size() - blankTiles;

	if(!_player->hasTiles(tiles, true)){ //they don't have correct tiles
		cout << 
		"\033[3;94mError: You don't have those tiles - please try again\033[0m";
		return false;
	}

	//checks bounds of board
	if((!h && (_x < 1 ||  _x+tiles.length()-blankTiles > board.getRows() + 1))
	 || (h && (_y < 1 || 
	 		_y + tiles.length()-blankTiles > board.getColumns() + 1))){
		cout << "\033[3;94mError: Out of bounds - please try again\033[0m";
		return false;
	}

	if(_player->getFirstMove()){ //checks first move is on starting square

		size_t sx = board.getStartX();
		size_t sy = board.getStartY();

		if((!h && (_x > sx || _x + tiles.length() < sx || _y != sy)) || (h &&
			(_y > sy || _y + tiles.length() < sy || _x != sx))){
			cout << 
				"\033[3;94mNot at starting square - please try again\033[0m";
			return false;
		}

	}

	std::vector<Tile*> hand = _player->takeTiles(tiles, true); //holds the hand

	size_t firstIndex, secondIndex;
	string dictionaryCheck = ""; //will be used to check word legality

	if(!h){ //checks legality of word w/ tiles around it

		size_t top = _x, bot = _x + tiles.length()-1 - blankTiles;

		for(size_t x = top; x <= bot; x++){
			if(board.getSquare(_y, x)->isOccupied()){
				bot++;
			}
		}

		//get indices of topmost and bottommost letter
		while(top > 1 && board.getSquare(_y, top-1)->isOccupied()){
			top--;
		}
		while(bot < board.getRows() && 
			board.getSquare(_y, bot+1)->isOccupied()){
			bot++;
		}

		int index = 0;

		for(size_t x = top; x <= bot; x++){ //visits every letter

			if(board.getSquare(_y, x)->isOccupied()){
				//if it's already occupied, just add pre-existing letter
				dictionaryCheck += board.getSquare(_y, x)->getLetter();
				wordPoints += board.getSquare(_y,x)->getScore(); //and points
			}
			else{
				if(tiles[index] == '?'){ //accounts for blanks
					index++;
				}
				dictionaryCheck += tiles[index];
				index++;
			}

			//this runs when there's a perprendicular word to check legality of
			if(!board.getSquare(_y,x)->isOccupied()){
				string newWord = "";
				size_t points = 0;
				size_t right = _y, left = _y;

				//gets left and right indices of the perpendicular word
				while(left>1 && board.getSquare(left-1,x)->isOccupied()){
					left--;
				}
				while(right<board.getColumns() && 
					board.getSquare(right+1,x)->isOccupied()){
					right++;
				}
				if(left == right){ //edge case
					continue;
				}
				//creates the newWord to check legality of
				for(size_t z = left; z <= right; z++){
					if(z == _y){
						newWord += tiles[index-1];
					}
					else{
						newWord += board.getSquare(z, x)->getLetter();
						points += board.getSquare(z,x)->getScore();
					}
				}
				_player->increasePoints(points); //gets points for other words
				if(!dictionary.isLegalWord(newWord)){
					cout << 
					"\033[3;94mError: Illegal word - please try again\033[0m";
					_player->addTiles(hand);
					return false;
				}
			}
		}

		if(dictionaryCheck[dictionaryCheck.length()-1] == '\0'){ //edge case
			string holder = "";
			for(size_t x = 0; x < dictionaryCheck.length()-1; x++){
				holder += dictionaryCheck[x];
			}
			dictionaryCheck = holder;
		}

		if(!dictionary.isLegalWord(dictionaryCheck)){ //if not legal word
			cout << "\033[3;94mError: Illegal word - please try again\033[0m";
			_player->addTiles(hand);
			return false;
		}

	}

	else{

		size_t left = _y, right = _y + tiles.length()-1 - blankTiles;

		for(size_t x = left; x <= right; x++){
			if(board.getSquare(x, _x)->isOccupied()){
				right++;
			}
		}

		//gets leftmost and rightmost indices of the word
		while(left > 1 && board.getSquare(left-1, _x)->isOccupied()){
			left--;
		}
		while(right < board.getColumns() &&
			board.getSquare(right+1, _x)->isOccupied()){
			right++;
		}

		int index = 0;

		for(size_t x = left; x <= right; x++){ //visits every letter

			if(board.getSquare(x, _x)->isOccupied()){
				//if it's occupied, just take that square's letter and points
				dictionaryCheck += board.getSquare(x, _x)->getLetter();
				wordPoints += board.getSquare(x,_x)->getScore();
			}
			else{
				if(tiles[index] == '?'){ //accounts for blanks
					index++;
				}
				dictionaryCheck += tiles[index];
				index++;
			}
			
			//accounts for new words created perpendicular to main word
			if(!board.getSquare(x,_x)->isOccupied()){
				string newWord = "";
				size_t points = 0;
				size_t bot = _x, top = _x;

				//gets topmost and bottommost indices of new word
				while(top>1 && board.getSquare(x,top-1)->isOccupied()){
					top--;
				}
				while(bot<board.getRows() && 
					board.getSquare(x, bot+1)->isOccupied()){
					bot++;
				}
				if(top == bot){ //edge case
					continue;
				}

				//creates the word to check legality of
				for(size_t z = top; z <= bot; z++){
					if(z == _x){
						newWord += tiles[index-1];
					}
					else{
						newWord += board.getSquare(x,z)->getLetter();
						points += board.getSquare(x,z)->getScore();
					}
				}

				_player->increasePoints(points); //get points for that word

				if(!dictionary.isLegalWord(newWord)){ //if not legal word
					cout << 
					"\033[3;94mError: Illegal word - please try again\033[0m";
					_player->addTiles(hand);
					return false;
				}
			}
		}

		if(dictionaryCheck[dictionaryCheck.length()-1] == '\0'){ //edge case
			string holder = "";
			for(size_t x = 0; x < dictionaryCheck.length()-1; x++){
				holder += dictionaryCheck[x];
			}
			dictionaryCheck = holder;
		}

		if(!dictionary.isLegalWord(dictionaryCheck)){ //if main word not legal
			cout << "\033[3;94mError: Illegal word - please try again\033[0m";
			_player->addTiles(hand);
			return false;
		}

	}

	//checks for bounding issues
	if(!_player->getFirstMove()){

		bool bounded = false;

		if(!h){ //if vertical

			//checks top and bottom squares
			if((_x-1>=1 && board.getSquare(_y, _x-1)->isOccupied()) ||
				(_x+tiles.length()<=board.getRows() && 
				board.getSquare(_y, _x+tiles.length())->
				isOccupied())){

				bounded = true;

			}

			//checks all borders
			for(int y = 0; y < actualLength; y++){

				if(_y+1 > board.getColumns()){
					if(board.getSquare(_y-1, _x+y)->isOccupied()){
						bounded = true;
					}
				}
				else if(_y-1 < 1){
					if(board.getSquare(_y+1, _x+y)->isOccupied()){
						bounded = true;
					}
				}
				else{
					if(board.getSquare(_y+1, _x+y)->isOccupied() ||
						board.getSquare(_y-1, _x+y)->isOccupied()){
						bounded = true;
					}
				}

			}

		}

		else{ //if horizontal

			//checks bounding of left or rightmost squares
			if((_y-1>=1 && board.getSquare(_y-1, _x)->isOccupied()) ||
				(_y+tiles.length()<=board.getColumns() && 
				board.getSquare(_y+tiles.length(), _x)->
				isOccupied())){

				bounded = true;

			}

			//cehcks all borders
			for(size_t y = 0; y < tiles.length() - blankTiles; y++){

				if(_x+1 > board.getRows()){
					if(board.getSquare(_y+y, _x-1)->isOccupied()){
						bounded = true;
					}
				}
				else if(_x-1 < 1){
					if(board.getSquare(_y+y, _x+1)->isOccupied()){
						bounded = true;
					}
				}
				else{
					if(board.getSquare(_y+y, _x+1)->isOccupied() ||
						board.getSquare(_y+y, _x-1)->isOccupied()){
						bounded = true;
					}
				}

			}

		}

		if(!bounded){ //the word isn't by a pre-existing tile
			cout << "\033[3;94mError: Not bounded - please try again\033[0m";
			_player->addTiles(hand);
			return false;
		}

	}

	int occupiedLetters = 0;
	bool start = true;

	//gets points of tiles - visits each tile on board for word
	for(unsigned int x = 1; x <= tiles.length() - blankTiles; x++){

		if(!h){ //assigns indices if vertical
			firstIndex = _y;
			secondIndex = _x + x - 1 + occupiedLetters;
		}

		else{ //asigns indicces if horizontal
			firstIndex = _y + x - 1 + occupiedLetters;
			secondIndex = _x;
		}

		//if it's occupied and this is the first letter, error
		if(board.getSquare(firstIndex, secondIndex)->isOccupied() && start){
			cout << 
			"\033[3;94mError: Starting square is occupied - please try again\033[0m";
			_player->addTiles(hand);
			return false;
		}

		start = false;

		//account for if there's a square already there
		if(board.getSquare(firstIndex, secondIndex)->isOccupied()){
			x--;
			occupiedLetters++;
			continue;
		}

		letterPoints = hand[index]->getPoints(); //otherwise add new points

		if(!h){ //checks for if it made more than one word
			if(!board.getSquare(firstIndex,secondIndex)->isOccupied() && 
				((firstIndex-1>0 && 
				board.getSquare(firstIndex-1, secondIndex)->isOccupied())
				|| (firstIndex+1<board.getRows() && 
				board.getSquare(firstIndex+1, secondIndex)->isOccupied()))){
				letterPoints *= 2;
			}
		}
		else{ //checks for if it made more than one word
			if(!board.getSquare(firstIndex,secondIndex)->isOccupied() &&
				((secondIndex-1>0 && 
				board.getSquare(firstIndex, secondIndex-1)->isOccupied())
				|| (secondIndex+1<board.getColumns() && 
				board.getSquare(firstIndex, secondIndex+1)->isOccupied()))){
				letterPoints *= 2;
			}
		}

		//the following block checks for multipliers
		if(board.getSquare(firstIndex, secondIndex)->getLMult() == 2){
			letterPoints *= 2;
		}

		else if(board.getSquare(firstIndex, secondIndex)->getLMult() == 3){
			letterPoints *= 3;
		}

		else if(board.getSquare(firstIndex, secondIndex)->getWMult() == 2){
			doubled++;
		}

		else if(board.getSquare(firstIndex, secondIndex)->getWMult() == 3){
			tripled++;
		}

		wordPoints += letterPoints; //add letter value to total points of word

		board.getSquare(firstIndex, secondIndex)->placeTile(hand[index]);
		index++;

	}

	while(doubled != 0){

		//double total points of their word - not other words formed though
		wordPoints *= 2;
		doubled--;

	}

	while(tripled != 0){

		//same as above but triple
		wordPoints *= 3;
		tripled--;

	}

	//if they used all of their tiles, they get bonus 50 points
	if(_player->getMaxTiles() == tiles.length()){

		wordPoints += 50;

	}

	_player->addTiles(bag.drawTiles(tiles.size() - blankTiles));
	_player->increasePoints(wordPoints);

	return true;

}

bool PlaceMove::tryPlace(Board & board, Bag & bag, Dictionary & dictionary){
	int actualLength;
	int blankTiles = 0; //used when drawing tiles back at the end of move
	for(size_t x = 0; x < tiles.length(); x++){ //counts blanks
		if(tiles[x] == '?'){
			blankTiles++;
		}
	}
	actualLength = tiles.size() - blankTiles;
	if(!_player->hasTiles(tiles, true)){ //they don't have correct tiles
		return false;
	}
	//checks bounds of board
	if((!h && (_x < 1 ||  _x+tiles.length()-blankTiles > board.getRows() + 1))
	 || (h && (_y < 1 || 
	 		_y + tiles.length()-blankTiles > board.getColumns() + 1))){
		return false;
	}
	if(_x > board.getRows() || _x < 1 || 
		_y > board.getColumns() || _y < 1 ||
		board.getSquare(_y,_x)->isOccupied()){
		return false;
	}
	if(_player->getFirstMove()){ //checks first move is on starting square
		size_t sx = board.getStartX();
		size_t sy = board.getStartY();
		if((!h && (_x > sx || _x + tiles.length() < sx || _y != sy)) || (h &&
			(_y > sy || _y + tiles.length() < sy || _x != sx))){
			return false;
		}
	}
	size_t firstIndex, secondIndex;
	string dictionaryCheck = ""; //will be used to check word legality
	if(!h){ //checks legality of word w/ tiles around it
		size_t top = _x, bot = _x + tiles.length()-1 - blankTiles;
		if(bot > board.getRows()){
			return false;
		}
		for(size_t x = top; x <= bot; x++){
			if(x > board.getRows()){
				break;
			}
			if(board.getSquare(_y, x)->isOccupied()){ //SEGFAULT HERE
				bot++;
			}
		}
		//get indices of topmost and bottommost letter
		while(top > 1 && board.getSquare(_y, top-1)->isOccupied()){
			top--;
		}
		while(bot < board.getRows() && 
			board.getSquare(_y, bot+1)->isOccupied()){
			bot++;
		}
		int index = 0;
		if(bot > board.getRows()){
			return false;
		}
		for(size_t x = top; x <= bot; x++){ //visits every letter
			if(board.getSquare(_y, x)->isOccupied()){
				//if it's already occupied, just add pre-existing letter
				dictionaryCheck += board.getSquare(_y, x)->getLetter();
			}
			else{
				if(tiles[index] == '?'){ //accounts for blanks
					index++;
				}
				dictionaryCheck += tiles[index];
				index++;
			}
			//this runs when there's a perprendicular word to check legality of
			if(!board.getSquare(_y,x)->isOccupied()){
				string newWord = "";
				size_t right = _y, left = _y;
				//gets left and right indices of the perpendicular word
				while(left>1 && board.getSquare(left-1,x)->isOccupied()){
					left--;
				}
				while(right<board.getColumns() && 
					board.getSquare(right+1,x)->isOccupied()){
					right++;
				}
				if(left == right){ //edge case
					continue;
				}
				//creates the newWord to check legality of
				for(size_t z = left; z <= right; z++){
					if(z == _y){
						newWord += tiles[index-1];
					}
					else{
						newWord += board.getSquare(z, x)->getLetter();
					}
				}
				if(!dictionary.isLegalWord(newWord)){
					return false;
				}
			}
		}
		if(dictionaryCheck[dictionaryCheck.length()-1] == '\0'){ //edge case
			string holder = "";
			for(size_t x = 0; x < dictionaryCheck.length()-1; x++){
				holder += dictionaryCheck[x];
			}
			dictionaryCheck = holder;
		}
		if(!dictionary.isLegalWord(dictionaryCheck)){ //if not legal word
			return false;
		}
	}
	else{
		size_t left = _y, right = _y + tiles.length()-1 - blankTiles;
		if(right > board.getColumns()){
			return false;
		}
		for(size_t x = left; x <= right; x++){
			if(x > right || x < 1 || x > board.getRows() || _x < 1
				|| _x > board.getRows()){
				break;
			}
			if(board.getSquare(x, _x)->isOccupied()){
				right++;
			}
		}
		//gets leftmost and rightmost indices of the word
		while(left > 1 && board.getSquare(left-1, _x)->isOccupied()){
			left--;
		}
		while(right < board.getColumns() &&
			board.getSquare(right+1, _x)->isOccupied()){
			right++;
		}
		int index = 0;
		if(right > board.getColumns()){
			return false;
		}
		for(size_t x = left; x <= right; x++){ //visits every letter
			if(board.getSquare(x, _x)->isOccupied()){
				//if it's occupied, just take that square's letter and points
				dictionaryCheck += board.getSquare(x, _x)->getLetter();
			}
			else{
				if(tiles[index] == '?'){ //accounts for blanks
					index++;
				}
				dictionaryCheck += tiles[index];
				index++;
			}
			//accounts for new words created perpendicular to main word
			if(!board.getSquare(x,_x)->isOccupied()){
				string newWord = "";
				size_t bot = _x, top = _x;
				//gets topmost and bottommost indices of new word
				while(top>1 && board.getSquare(x,top-1)->isOccupied()){
					top--;
				}
				while(bot<board.getRows() && 
					board.getSquare(x, bot+1)->isOccupied()){
					bot++;
				}
				if(top == bot){ //edge case
					continue;
				}
				//creates the word to check legality of
				for(size_t z = top; z <= bot; z++){
					if(z == _x){
						newWord += tiles[index-1];
					}
					else{
						newWord += board.getSquare(x,z)->getLetter();
					}
				}
				if(!dictionary.isLegalWord(newWord)){ //if not legal word
					return false;
				}
			}
		}
		if(dictionaryCheck[dictionaryCheck.length()-1] == '\0'){ //edge case
			string holder = "";
			for(size_t x = 0; x < dictionaryCheck.length()-1; x++){
				holder += dictionaryCheck[x];
			}
			dictionaryCheck = holder;
		}
		if(!dictionary.isLegalWord(dictionaryCheck)){ //if main word not legal
			return false;
		}
	}
	//checks for bounding issues
	if(!_player->getFirstMove()){
		bool bounded = false;
		if(!h){ //if vertical
			//checks top and bottom squares
			if((_x-1>=1 && board.getSquare(_y, _x-1)->isOccupied()) ||
				(_x+tiles.length()<=board.getRows() && 
				board.getSquare(_y, _x+tiles.length())->
				isOccupied())){
				bounded = true;
			}
			//checks all borders
			for(int y = 0; y < actualLength; y++){
				if(_y+1 > board.getColumns()){
					if(board.getSquare(_y-1, _x+y)->isOccupied()){
						bounded = true;
					}
				}
				else if(_y-1 < 1){
					if(board.getSquare(_y+1, _x+y)->isOccupied()){
						bounded = true;
					}
				}
				else{
					if(board.getSquare(_y+1, _x+y)->isOccupied() ||
						board.getSquare(_y-1, _x+y)->isOccupied()){
						bounded = true;
					}
				}
			}
		}
		else{ //if horizontal
			//checks bounding of left or rightmost squares
			if((_y-1>=1 && board.getSquare(_y-1, _x)->isOccupied()) ||
				(_y+tiles.length()<=board.getColumns() && 
				board.getSquare(_y+tiles.length(), _x)->
				isOccupied())){
				bounded = true;
			}
			//cehcks all borders
			for(size_t y = 0; y < tiles.length(); y++){
				if(_x+1 > board.getRows()){
					if(board.getSquare(_y+y, _x-1)->isOccupied()){
						bounded = true;
					}
				}
				else if(_x-1 < 1){
					if(board.getSquare(_y+y, _x+1)->isOccupied()){
						bounded = true;
					}
				}
				else{
					if(_y+y > board.getColumns() || _x+1 > board.getRows() ||
						_x+1 < 1 || _x-1 < 1){
						continue;
					}
					// std::cout << _y+y << " " << _x+1 << std::endl;
					if(board.getSquare(_y+y, _x+1)->isOccupied() ||
						board.getSquare(_y+y, _x-1)->isOccupied()){
						bounded = true;
					}
				}
			}
		}
		if(!bounded){ //the word isn't by a pre-existing tile
			return false;
		}
	}
	int occupiedLetters = 0;
	bool start = true;
	//gets points of tiles - visits each tile on board for word
	for(unsigned int x = 1; x <= tiles.length() - blankTiles; x++){
		if(!h){ //assigns indices if vertical
			firstIndex = _y;
			secondIndex = _x + x - 1 + occupiedLetters;
		}
		else{ //asigns indicces if horizontal
			firstIndex = _y + x - 1 + occupiedLetters;
			secondIndex = _x;
		}
		//if it's occupied and this is the first letter, error
		if(board.getSquare(firstIndex, secondIndex)->isOccupied() && start){
			return false;
		}
		start = false;
	}
	return true;

}

int PlaceMove::getPoints(Board & board, Bag & bag, Dictionary & dictionary){

	int doubled = 0; //times they got a double word multiplier
	int tripled = 0; //times they got a triple word multiplier
	int wordPoints = 0, letterPoints = 0, index = 0;
	int blankTiles = 0; //used when drawing tiles back at the end of move

	for(size_t x = 0; x < tiles.length(); x++){ //counts blanks
		if(tiles[x] == '?'){
			blankTiles++;
		}
	}

	std::vector<Tile*> hand = _player->takeTiles(tiles, true); //holds the hand

	size_t firstIndex, secondIndex;
	string dictionaryCheck = ""; //will be used to check word legality

	if(!h){ //checks legality of word w/ tiles around it

		size_t top = _x, bot = _x + tiles.length()-1 - blankTiles;

		for(size_t x = top; x <= bot; x++){
			if(board.getSquare(_y, x)->isOccupied()){
				bot++;
			}
		}

		//get indices of topmost and bottommost letter
		while(top > 1 && board.getSquare(_y, top-1)->isOccupied()){
			top--;
		}
		while(bot < board.getRows() && 
			board.getSquare(_y, bot+1)->isOccupied()){
			bot++;
		}

		int index = 0;

		for(size_t x = top; x <= bot; x++){ //visits every letter
			if(board.getSquare(_y, x)->isOccupied()){
				//if it's already occupied, just add pre-existing letter
				wordPoints += board.getSquare(_y,x)->getScore(); //and points
			}
			else{
				if(tiles[index] == '?'){ //accounts for blanks
					index++;
				}
				index++;
			}

			//this runs when there's a perprendicular word to check legality of
			if(!board.getSquare(_y,x)->isOccupied()){
				size_t points = 0;
				size_t right = _y, left = _y;

				//gets left and right indices of the perpendicular word
				while(left>1 && board.getSquare(left-1,x)->isOccupied()){
					left--;
				}
				while(right<board.getColumns() && 
					board.getSquare(right+1,x)->isOccupied()){
					right++;
				}
				if(left == right){ //edge case
					continue;
				}
				//creates the newWord to check legality of
				for(size_t z = left; z <= right; z++){
					if(z != _y){{
						points += board.getSquare(z,x)->getScore();
					}
				}
				wordPoints += points;
			}
		  }
		}
	}
	else{
		size_t left = _y, right = _y + tiles.length()-1 - blankTiles;
		for(size_t x = left; x <= right; x++){
			if(board.getSquare(x, _x)->isOccupied()){
				right++;
			}
		}
		//gets leftmost and rightmost indices of the word
		while(left > 1 && board.getSquare(left-1, _x)->isOccupied()){
			left--;
		}
		while(right < board.getColumns() &&
			board.getSquare(right+1, _x)->isOccupied()){
			right++;
		}

		int index = 0;

		for(size_t x = left; x <= right; x++){ //visits every letter

			if(board.getSquare(x, _x)->isOccupied()){
				//if it's occupied, just take that square's letter and points
				wordPoints += board.getSquare(x,_x)->getScore();
			}
			else{
				if(tiles[index] == '?'){ //accounts for blanks
					index++;
				}
				index++;
			}
			
			//accounts for new words created perpendicular to main word
			if(!board.getSquare(x,_x)->isOccupied()){
				size_t points = 0;
				size_t bot = _x, top = _x;
				//gets topmost and bottommost indices of new word
				while(top>1 && board.getSquare(x,top-1)->isOccupied()){
					top--;
				}
				while(bot<board.getRows() && 
					board.getSquare(x, bot+1)->isOccupied()){
					bot++;
				}
				if(top == bot){ //edge case
					continue;
				}
				//creates the word to check legality of
				for(size_t z = top; z <= bot; z++){
					if(z != _x){
						points += board.getSquare(x,z)->getScore();
					}
				}
				wordPoints += points;
			}
		}
	}
	int occupiedLetters = 0;
	//gets points of tiles - visits each tile on board for word
	for(unsigned int x = 1; x <= tiles.length() - blankTiles; x++){
		if(!h){ //assigns indices if vertical
			firstIndex = _y;
			secondIndex = _x + x - 1 + occupiedLetters;
		}
		else{ //asigns indicces if horizontal
			firstIndex = _y + x - 1 + occupiedLetters;
			secondIndex = _x;
		}
		//account for if there's a square already there
		if(board.getSquare(firstIndex, secondIndex)->isOccupied()){
			x--;
			occupiedLetters++;
			continue;
		}
		letterPoints = hand[index]->getPoints(); //otherwise add new points
		if(!h){ //checks for if it made more than one word
			if(!board.getSquare(firstIndex,secondIndex)->isOccupied() && 
				((firstIndex-1>0 && 
				board.getSquare(firstIndex-1, secondIndex)->isOccupied())
				|| (firstIndex+1<board.getRows() && 
				board.getSquare(firstIndex+1, secondIndex)->isOccupied()))){
				letterPoints *= 2;
			}
		}
		else{ //checks for if it made more than one word
			if(!board.getSquare(firstIndex,secondIndex)->isOccupied() &&
				((secondIndex-1>0 && 
				board.getSquare(firstIndex, secondIndex-1)->isOccupied())
				|| (secondIndex+1<board.getColumns() && 
				board.getSquare(firstIndex, secondIndex+1)->isOccupied()))){
				letterPoints *= 2;
			}
		}
		//the following block checks for multipliers
		if(board.getSquare(firstIndex, secondIndex)->getLMult() == 2){
			letterPoints *= 2;
		}
		else if(board.getSquare(firstIndex, secondIndex)->getLMult() == 3){
			letterPoints *= 3;
		}
		else if(board.getSquare(firstIndex, secondIndex)->getWMult() == 2){
			doubled++;
		}
		else if(board.getSquare(firstIndex, secondIndex)->getWMult() == 3){
			tripled++;
		}
		wordPoints += letterPoints; //add letter value to total points of word
		index++;
	}
	while(doubled != 0){ //doubled
		wordPoints *= 2;
		doubled--;
	}
	while(tripled != 0){ //triple
		wordPoints *= 3;
		tripled--;
	}
	//if they used all of their tiles, they get bonus 50 points
	if(_player->getMaxTiles() == tiles.length()){
		wordPoints += 50;
	}
	_player->addTiles(hand);
	return wordPoints;
}