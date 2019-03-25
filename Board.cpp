#include "Board.h"
#include <string>
#include <fstream>
#include <sstream>
#include "Exceptions.h"

Board::Board(std::string board_file_namey){

	initializedWithTiles = false;
	std::ifstream boardFileStream(board_file_namey);

	if(boardFileStream.fail()){
		throw FileException("BOARD");
	}

	std::string line;
	getline(boardFileStream, line);
	std::stringstream ss1(line);

	ss1 >> height >> width; //read in the dimensions

	getline(boardFileStream, line);
	std::stringstream ss2(line);

	ss2 >> starty >> startx; //read in the starting square

	//dynamically allocating the board
	board = new Square**[height];

	for(unsigned int x = 0; x < height; x++){

		board[x] = new Square*[width];

	}

	unsigned int index = 0;
	char ch;

	while(getline(boardFileStream, line)){ //reading in board layout

		std::stringstream ss(line);

		for(unsigned int x = 0; x < height; x++){ //goes through each line

			bool start = false; //if it's the starting square

			ss >> ch; //read in potential multiplier

			if(x == starty-1 && index == startx-1){
				start = true;
			}
			
			if(ch == 't'){
				board[x][index] = new Square(1, 3, start);
			}

			else if(ch == 'd'){
				board[x][index] = new Square(1, 2, start);
			}

			else if(ch == '2'){
				board[x][index] = new Square(2, 1, start);
			}

			else if(ch == '3'){
				board[x][index] = new Square(3, 1, start);
			}

			else{
				board[x][index] = new Square(1, 1, start);
			}

		}

		index++;

	}

}

Board::~Board(){

	//deconstructing the board
	for(size_t x = 0; x < height; x++){

		for(size_t y = 0; y < width; y++){

			if(board[x][y]){
				delete[] board[x][y];
			}

		}

	}

	for(unsigned int x = 0; x < height; x++){

		delete[] board[x];

	}

	delete[] board;

}

Square * Board::getSquare (size_t x, size_t y) const{

	return board[x-1][y-1];

}

size_t Board::getRows() const{

	return height;

}

size_t Board::getColumns() const{

	return width;

}

bool Board::isEmpty(){ //returns if the board is empty

	for(size_t x = 0; x < height; x++){

		for(size_t y = 0; y < width; y++){

			if(board[x][y] != nullptr){

				return false;

			}

		}

	}

	return true;

}

void Board::initialize(std::string file){

	if(file == ""){
		return;
	}

	std::ifstream filestream(file);

	if(filestream.fail()){
		throw FileException("BOARD");
	}

	else{


		initializedWithTiles = true;
		std::string line;

		for(size_t x = 0; x < height; x++){

			std::getline(filestream,line);
			char letter1, letter2, letter3;
			//int points;
			stringstream ss(line);
			
			for(size_t y = 0; y < width; y++){
				ss >> letter1 >> letter2 >> letter3;
				if(letter1 != '.'){
					if(letter1 < 91){ //handles uppercase
						letter1 = letter1 + 32;
					}
					initializedWithTiles = true;
					int points = ((letter2-48)*10) + (letter3 - 48);
					Tile* added = new Tile(letter1, points);
					getSquare(y+1, x+1)->placeTile(added);
					//MAKE SURE THE DIMENSIONS ARE CORRECT
				}
			}
		}
	}
}