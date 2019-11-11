#include "game.hpp"
#include <random>
#include <vector>
#include <map>
#include <random>
#include <algorithm>
#include <utility>
#include <sstream>
#include <ctime>

using std::vector;
using std::map;
using std::pair;

vector<Coord> Game::get_adjacent(Coord c) const {
	
	auto [x, y] = c;

	// vector of coords adjacent to c
	vector<Coord> adjacent_points;
	adjacent_points.push_back(Coord{x-1, y-1});
	adjacent_points.push_back(Coord{x-1, y});
	adjacent_points.push_back(Coord{x-1, y+1});
	adjacent_points.push_back(Coord{x, y-1});
	adjacent_points.push_back(Coord{x, y+1});
	adjacent_points.push_back(Coord{x+1, y-1});
	adjacent_points.push_back(Coord{x+1, y});
	adjacent_points.push_back(Coord{x+1, y+1});

	// vector of valid subset of adjacent_points
	vector<Coord> valid_adjacent_points;
	copy_if(adjacent_points.begin(), adjacent_points.end(),
		back_inserter(valid_adjacent_points), [this](Coord c) {
			return this->is_valid(c);
	});
	return valid_adjacent_points;
}


map<Coord, Cell> Game::build_board(int width, int height, double density, Coord c) {
	// Initializing Mersenne Twister
	std::mt19937 rand;
	rand.seed(std::time(nullptr));

	// Initializing board map
	map<Coord, Cell> result;
	for (int w = 0; w < width; ++w) {
		for (int h = 0; h < height; ++h) {
			result[Coord(w, h)];
		}
	}

	// Off-limits squares for mines
	auto safe = get_adjacent(c);

	// Putting mines on board map
	num_mines = 0;
	while (num_mines < density * width * height) {
		Coord coord = {rand() % width, rand() % height};
		Cell& cell = result[coord];
		if (!cell.mine && find(safe.begin(), safe.end(), coord) == safe.end()) {
			cell.mine = true;
			++num_mines;
		}
	}
	
	return result;
}

size_t Game::mines_adjacent(Coord coord) const {
	vector<Coord> adj = get_adjacent(coord);
	size_t mines = count_if(adj.begin(), adj.end(), [this](Coord c) {
		/*lambda*/return this->board.at(c).mine;
	});
	return mines;
}

int &Game::x() {return current.first;}
int &Game::y() {return current.second;}

int Game::get_x() const {return current.first;}
int Game::get_y() const {return current.second;}

int Game::get_width() const {return width;}
int Game::get_height() const {return height;}

bool Game::is_running() const {return running;}
bool Game::is_ongoing() const {return ongoing;}
bool Game::is_won() const {return won;}

std::stringstream Game::status() const {
	using std::stringstream;
	stringstream s;

	// if board is initialized, print it
	if (is_board_initialized) for (auto [coord, cell] : board) {
		if (cell.revealed) {
			size_t mines = mines_adjacent(coord);
			if (cell.mine) s << '*';
			else if (mines) s << mines;
			else s << ' ';
		} else if (cell.flagged) s << '#';
		else s << '=';
	// if board is not initialized, print empty board
	} else for (int i = 0; i < width * height; ++i) s << '=';

	return s;
}

bool Game::is_valid(Coord c) const {
	return c.first < width && c.second < height && \
		c.first >= 0 && c.second >= 0;
}

void Game::cursor_up() {
	if (y() > 0) --y();
}
void Game::cursor_down() {
	if (y() < height - 1) ++y();
}
void Game::cursor_left() {
	if (x() > 0) --x();
}
void Game::cursor_right() {
	if (x() < width - 1) ++x();
}
void Game::cursor_flag() {
	if (board[current].flagged) board[current].flagged = false;
	else board[current].flagged = true;
}
void Game::cursor_clear() {
	reveal(current);
}
void Game::reveal(Coord coord) {
	if (!is_valid(coord)) return;

	// Build board that gives the player a fair start
	while (!is_board_initialized) {
		board = build_board(width, height, density, coord);
		Cell& cell = board[coord];
		if (!cell.mine ){//&& !mines_adjacent(coord)) {
			is_board_initialized = true;
		}
	}

	Cell& cell = board[coord];

	// Do nothing if flagged or revealed
	if (cell.flagged || cell.revealed) return;

	cell.revealed = true;
	++cells_revealed;
	if (cell.mine) {
		game_over();
		return;
	}
	
	if (cells_revealed == width * height - num_mines) win();

	if (!mines_adjacent(coord)) {
		for (Coord c : get_adjacent(coord)) {
			reveal(c);
		}
	}
}

void Game::game_over() {
	for (auto& [coord, cell] : board) {
		cell.revealed = true;
	}
	ongoing = false;
}

void Game::win() {
	ongoing = false;
	won = true;
}

void Game::quit() {
	running = false;
}
