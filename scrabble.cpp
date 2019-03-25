#include <fstream>
#include <string>
#include <iostream>
#include "Player.cpp"
#include <vector>
#include <sstream>
#include "Bag.cpp"
#include "ConsolePrinter.cpp"
#include "Board.cpp"
#include "Dictionary.cpp"
#include "Move.cpp"
#include "cpus.cpp"
#include "cpul.cpp"

using namespace std;

//gets winner from the vector of player pointers
std::vector<Player*> getWinner(std::vector<Player*> players){

	std::vector<Player*> winners;
	Player* p = players[0];

	if(players.size() == 1){ //if there's only one player, they win

		winners.push_back(players[0]);
		return winners;

	}

	//finds highest-point-weilding player
	for(size_t x = 1; x < players.size(); x++){

		if(players[x]->getPoints() > p->getPoints()){

			p = players[x];

		}

	}

	//finds any other players with same score
	for(size_t x = 0; x < players.size(); x++){
		if(players[x]->getPoints() == p->getPoints()){

			winners.push_back(players[x]);

		}
	}

	return winners;

}

void end(Player* player, std::vector<Player*> players){

	int totalAdded = 0;
	int subtract = 0;

	//subtracts proper points from players' hands
	for(size_t z = 0; z < players.size(); z++){

		if(player != players[z]){

			std::set<Tile*> holder = players[z]->getHandTiles();

			for(std::set<Tile*>::iterator it = holder.begin();
				it != holder.end(); it++){

				subtract += (*it)->getPoints();

			}
			players[z]->increasePoints(-1*subtract);
		}

		totalAdded += subtract;
		subtract = 0;

	}

	//winner gets points subtracted from everyone else's hands
	player->increasePoints(totalAdded);

	std::vector<Player*> winners = getWinner(players);

	//displays the winners
	cout << rang::fg::cyan << rang::style::italic;

	if(winners.size() == 1){
		cout << "Winner: ";
	}
	else{
		cout << "Winners: ";
	}

	for(size_t y = 0; y < winners.size(); y++){
		cout << winners[y]->getName() << " ";
	}
	cout << endl << endl << "Final Scores: ";
	for(unsigned int y = 0; y < players.size(); y++){
		cout << players[y]->getName() << ": " << 
			players[y]->getPoints() << " ";
	}
	cout << rang::fg::reset << rang::style::reset << endl;

}

int main(int argc, char* argv[]){
	ifstream configFile(argv[1]);
	string line, tilefile, dictfile, boardfile, initfile = "";
	size_t maxTiles;
	uint32_t seed;
	while(getline(configFile, line)){ //reads in config file
		stringstream ss(line);
		string label;
		ss >> label;

		if(label == "HANDSIZE:"){
			ss >> maxTiles;
		}
		else if(label == "TILES:"){
			ss >> tilefile;
		}
		else if(label == "DICTIONARY:"){
			ss >> dictfile;
		}
		else if(label == "BOARD:"){
			ss >> boardfile;
		}
		else if(label == "SEED:"){
			ss >> seed;
		}
		else if(label == "INIT:"){
			ss >> initfile;
		}
	}
	int playerCount;
	cout << "How many players? ";
	cin >> playerCount;
	while(playerCount<1 || playerCount > 8){ //if incorrect player count
		cout << "\033[3;94mPlayer count must be 1-8 - Re-enter: \033[0m";
		cin >> playerCount;
	}
	string name;
	vector<Player*> players;
	//creates necessary objects for game
	Bag* letterBag = new Bag(tilefile, seed);
	Dictionary dict = Dictionary(dictfile);
	ConsolePrinter print;
	Board* board = new Board(boardfile);
	int aiCount = 0;
	board->initialize(initfile);
	getline(cin, line);

	//asks each player for their name
	for(int x = 0; x < playerCount; x++){
		cout << "Name? ";
		getline(cin,name);
		if((name[0] == 'c' || name[0] == 'C') && (name[1] == 'p' || 
			name[1] == 'P') && (name[2] == 'u' || name[2] == 'U') &&
			(name[3] == 's' || name[3] == 'S')){
			CPUS* newplayer = new CPUS(name, maxTiles);
			newplayer->addTiles(letterBag->drawTiles(maxTiles));
			players.push_back(newplayer);
			aiCount++;
		}
		else if((name[0] == 'c' || name[0] == 'C') && (name[1] == 'p' || 
			name[1] == 'P') && (name[2] == 'u' || name[2] == 'U') &&
			(name[3] == 'l' || name[3] == 'L')){
			CPUL* newplayer = new CPUL(name, maxTiles);
			newplayer->addTiles(letterBag->drawTiles(maxTiles));
			players.push_back(newplayer);
			aiCount++;
		}
		else{
			Player* newplayer = new Player(name, maxTiles);
			newplayer->addTiles(letterBag->drawTiles(maxTiles));
			players.push_back(newplayer);
		}
	}
	unsigned int passes = 0; //for the end condition
	if(!(board->getInitialized())){
		players[0]->setFirstMove(true);
	}
	while(true){
		for(unsigned int x = 0; x < players.size(); x++){
			//displays current state of the game
			print.printBoard(*board);
			print.printHand(*players[x]);
			cout << endl << endl << "Current Score: ";
			for(unsigned int y = 0; y < players.size(); y++){
				cout << players[y]->getName() << ": " << 
					players[y]->getPoints() << " ";
			}
			cout << endl;
			std::string move;

			//Have if statement with a break here for CPUS
			if(players[x]->isaCPUS()){
				players[x]->findBestMove(letterBag, board, dict);
				if(players[x]->getHandTiles().empty() && 
					letterBag->tilesRemaining() == 0){
					end(players[x], players);
					return 0;
				}
				print.printBoard(*board);
				print.printHand(*players[x]);
				cout << endl << endl << "Current Score: ";
				for(unsigned int y = 0; y < players.size(); y++){
					cout << players[y]->getName() << ": " << 
						players[y]->getPoints() << " ";
				}
				cout << endl << players[x]->getName() << "'s move complete";
				cout << endl << "Press [ENTER] to continue" << endl;
				cin.ignore(1000, '\n');
				continue;
			}
			//Have if statement with a break here for CPUL
			else if(players[x]->isaCPUL()){
				players[x]->findBestMove(letterBag, board, dict);
				if(players[x]->getHandTiles().empty() && 
					letterBag->tilesRemaining() == 0){
					end(players[x], players);
					return 0;
				}
				print.printBoard(*board);
				print.printHand(*players[x]);
				cout << endl << endl << "Current Score: ";
				for(unsigned int y = 0; y < players.size(); y++){
					cout << players[y]->getName() << ": " << 
						players[y]->getPoints() << " ";
				}
				cout << endl << players[x]->getName() << "'s move complete";
				cout << endl << "Press [ENTER] to continue" << endl;
				cin.ignore(1000, '\n');
				continue;
			}
			cout << endl << players[x]->getName() << 
				", what would you like to do? ";
			//gets the player's hand
			std::set<Tile*> holder = players[x]->getHandTiles();
			getline(cin, line); //reads in their move choice
			Move* choice = Move::parseMove(line, *players[x]);

			while(choice == nullptr){ //if it's not valid...
				cout << "\033[3;94mIncorrect syntax - please try again\033[0m";
				cout << endl << players[x]->getName() 
					<< ", what would you like to do? ";
				getline(cin, line);
				choice = Move::parseMove(line, *players[x]);
			}
			//checks for second / more errors
			while(choice == nullptr||!choice->execute(*board, *letterBag,dict)){
				if(choice == nullptr){
					cout << 
					"\033[3;94mIncorrect syntax - please try again\033[0m";
				}
				cout << endl << players[x]->getName() << 
					", what would you like to do? ";
				getline(cin, line);
				choice = Move::parseMove(line, *players[x]);
			}
			if((choice->isPass() || choice->isExchange()) && players[x]->getFirstMove()){
				players[x]->setFirstMove(false);
				if(x == players.size()-1){
					players[0]->setFirstMove(true);
				}
				else{
					players[x+1]->setFirstMove(true);
				}
			}
			else{
				players[x]->setFirstMove(false);
			}
			if(choice->getPass()){ //checks for passing end condition
				passes++;
				if(passes == players.size()-aiCount){ //everyone has passed
					int subtract = 0;
					//subtracts points still in hand from final score
					for(size_t z = 0; z < players.size(); z++){
						std::set<Tile*> holder = players[z]->getHandTiles();
						for(std::set<Tile*>::iterator it = holder.begin();
							it != holder.end(); it++){
							subtract += (*it)->getPoints();
						}
						players[z]->increasePoints(-1*subtract);
						subtract = 0;
					}
					//gets the winning players
					std::vector<Player*> winners = getWinner(players);
					cout << rang::fg::cyan << rang::style::italic;
					//displays winners
					if(winners.size() == 1){
						cout << "Winner: ";
					}
					else{
						cout << "Winners: ";
					}
					for(size_t y = 0; y < winners.size(); y++){
						cout << winners[y]->getName() << " ";
					}
					cout << endl << endl << "Final Scores: ";
					for(unsigned int y = 0; y < players.size(); y++){
						cout << players[y]->getName() << ": " << 
							players[y]->getPoints() << " ";
					}
					cout << rang::fg::reset << rang::style::reset << endl;
					return 0;
				}
			}
			else{
				passes = 0;
			}
			if(choice->isWord()){ //the first player has made their first move
				players[0]->setFirstMove(false);
			}
			delete choice;
			//other ending condition
			if(players[x]->getHandTiles().empty() && 
				letterBag->tilesRemaining() == 0){
					end(players[x], players);
					return 0;
			}
			//displays current state of the board
			print.printBoard(*board);
			print.printHand(*players[x]);
			cout << endl << endl << "Current Score: ";
			for(unsigned int y = 0; y < players.size(); y++){
				cout << players[y]->getName() << ": " << 
					players[y]->getPoints() << " ";
			}
			cout << endl << "Press [ENTER] to continue" << endl;
			cin.ignore(1000, '\n');
		}
	}
	return 0;
}