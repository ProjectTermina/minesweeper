#include <map>
#include <vector>
#include <utility>
#include <sstream>

using std::map;
using std::vector;
using std::pair;
using std::size_t;

struct Cell {
	bool revealed = false;
	bool flagged = false;
	bool mine = false;
};

using Coord = pair<int, int>;

namespace std{
template<>
struct std::less<Coord> {
	bool operator()(const Coord& c1, const Coord& c2) const {
		if (c1.second < c2.second) return true;
		else if (c1.second == c2.second && c1.first < c2.first) return true;
		else return false;
	}
};
}


class Game {
private:
	int width;
	int height;
	double density;
	bool is_board_initialized = false;
	map<Coord, Cell> board;
	Coord current;
	bool ongoing;
	bool running;

	int &x();
	int &y();

	map<Coord, Cell> build_board(int width, int height, double density, Coord c);

	size_t mines_adjacent(Coord coord) const;
	void reveal(Coord coord);
	bool is_valid(Coord) const;
	vector<Coord> get_adjacent(Coord c) const;

public:
	Game() :width(15), height(15), density(.20),
		current({7, 7}), ongoing(true), running(true) {}
	Game(int width, int height, double density = .20) : 
		width(width), height(height), density(density),
		current({width/2, height/2}), ongoing(true), running(true) {}

	int get_x() const;
	int get_y() const;
	int get_width() const;
	int get_height() const;
	bool is_running() const;
	bool is_ongoing() const;
	std::stringstream status() const;

	void cursor_up();
	void cursor_down();
	void cursor_left();
	void cursor_right();
	void cursor_flag();
	void cursor_clear();
	void game_over();
	void quit();
};
