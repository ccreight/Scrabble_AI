#include "Player.h"
#include <string>
#include <set>
#include <utility>

class CPUL : public Player{

	public:
		CPUL(std::string const & name, size_t maxTiles);
	protected:
		std::vector<std::string> playWord(std::set<Tile*> hand, 
			Square* square, int x, int y, Dictionary& dict);
		void findBestMove(Bag* bag, Board* board, Dictionary& dict);
		void getWords(std::vector<char> letters, 
		std::set<std::string>& words, Dictionary& dict, char onBoard);

};