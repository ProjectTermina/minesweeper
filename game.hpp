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

using Coord = pair<size_t, size_t>;

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
map<Coord, Cell> build_board(size_t width, size_t height, double density);

class Game {
private:
	size_t width;
	size_t height;
	map<Coord, Cell> board;
	Coord current;
	bool ongoing;
	bool running;

	size_t &x();
	size_t &y();

	size_t mines_adjacent(Coord coord) const;
	void reveal(Coord coord);
	bool is_valid(Coord) const;
	vector<Coord> get_adjacent(Coord c) const;

public:
	Game() :width(15), height(15), 
		board(build_board(15, 15, .20)), current({7, 7}),
		ongoing(true), running(true) {}
	Game(size_t width, size_t height, double density = .20) : 
		width(width), height(height),
		board(build_board(width, height, density)),
		current({width/2, height/2}), ongoing(true), running(true) {}

	size_t get_x() const;
	size_t get_y() const;
	size_t get_width() const;
	size_t get_height() const;
	bool is_running() const;
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
