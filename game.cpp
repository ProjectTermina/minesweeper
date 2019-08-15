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
	vector<Coord> potentials;
	potentials.push_back(Coord{x-1, y-1});
	potentials.push_back(Coord{x-1, y});
	potentials.push_back(Coord{x-1, y+1});
	potentials.push_back(Coord{x, y-1});
	potentials.push_back(Coord{x, y+1});
	potentials.push_back(Coord{x+1, y-1});
	potentials.push_back(Coord{x+1, y});
	potentials.push_back(Coord{x+1, y+1});
	vector<Coord> result;
	copy_if(potentials.begin(), potentials.end(),
		back_inserter(result), [this](Coord c) {
			size_t x = c.first;
			size_t y = c.second;
			return ( x < this->get_width() && y < this->get_height() );
		});
	return result;
}


map<Coord, Cell> build_board(size_t width, size_t height, double density) {
	std::mt19937 rand;	// initializing functor named rand
	rand.seed(std::time(nullptr));
	map<Coord, Cell> result;

	size_t mined = 0;
	for (int w = 0; w < width; ++w) {
		for (int h = 0; h < height; ++h) {
			result[Coord(w, h)];
		}
	 }
	while (mined < density * width * height) {
		Coord coord = {rand() % width, rand() % height};
		Cell& cell = result[coord];
		if (!cell.mine) {
			cell.mine = true;
			++mined;
		}
	}
	return result;
}

size_t Game::mines_adjacent(Coord coord) const {
	vector<Coord> adj = get_adjacent(coord);
	size_t mines = count_if(adj.begin(), adj.end(),
		[this](Coord c){/*lambda*/return this->board.at(c).mine;}
	);
	return mines;
}

size_t &Game::x() {return current.first;}
size_t &Game::y() {return current.second;}

size_t Game::get_x() const {return current.first;}
size_t Game::get_y() const {return current.second;}

size_t Game::get_width() const {return width;}
size_t Game::get_height() const {return height;}

bool Game::is_running() const {return running;}
bool Game::is_ongoing() const {return ongoing;}

std::stringstream Game::status() const {
	using std::stringstream;
	stringstream s;
	
	for (auto [coord, cell] : board) {
		if (cell.revealed) {
			size_t mines = mines_adjacent(coord);
			if (cell.mine) s << '*';
			else if (mines) s << mines;
			else s << ' ';
		} else if (cell.flagged) s << '#';
		else s << '=';
	}
	return s;
}

bool Game::is_valid(Coord c) const {
	return c.first < width && c.second < height;
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
	if (is_valid(coord)) {
		Cell& cell = board[coord];
		if (cell.flagged || cell.revealed) return;

		cell.revealed = true;
		if (cell.mine) {
			game_over();
			return;
		}

		if (!mines_adjacent(coord)) {
			for (Coord c : get_adjacent(coord)) {
				reveal(c);
			}
		}
	} else return;
}

void Game::game_over() {
	for (auto& [coord, cell] : board) {
		cell.revealed = true;
	}
	ongoing = false;
}

void Game::quit() {
	running = false;
}
