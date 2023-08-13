// Supposedly, a crumb is the base-4 equivalent of base-2's bit, base-3's trit or base-256's byte: https://en.wikipedia.org/wiki/Quaternary_numeral_system#Relation_to_binary_and_hexadecimal

#include "board.hpp"

#include <stdexcept>
#include <format>
#include <vector>
#include <optional>
#include <algorithm>

//#include <iostream>

using namespace Constants;

// Utilities

using RepType = std::bitset<2 * BOARD_WIDTH * BOARD_HEIGHT>;

void SetCrumb(RepType& of, const size_t at, const CellState to) {
	of.set(at * 2 + 1, to != CellState::EMPTY);
	of.set(at * 2, to == CellState::RED);
}

CellState GetState(const RepType& of, const size_t at) {
	if (!of.test(at * 2 + 1)) 
		return CellState::EMPTY;
	return of.test(at * 2) ? CellState::RED : CellState::BLUE;
}

Board::Board() noexcept : cells{} {}

Board::Board(const std::string_view input) {
	if (input.length() != BOARD_WIDTH * BOARD_HEIGHT) {
		throw std::runtime_error("Could not construct Board : bad input length");
	}
	for (size_t i = 0; i < BOARD_WIDTH * BOARD_HEIGHT; ++i) {
		switch (input[i]) {
		case '*':
			SetCrumb(cells, i, CellState::EMPTY);
			break;
		case 'B':
			SetCrumb(cells, i, CellState::BLUE);
			break;
		case 'R':
			SetCrumb(cells, i, CellState::RED);
			break;
		default: throw std::runtime_error(std::format("Could not construct Board: bad input containing \'{}\'", input[i]));
		}
	}
}

bool Board::BlueWin() const {
	return std::any_of(FivesBegin(), FivesEnd(), [](const auto value) -> bool {
		return value == -5;
	});
}

bool Board::RedWin() const {
	return std::any_of(FivesBegin(), FivesEnd(), [](const auto value) -> bool {
		return value == 5;
	});
}

CellState Board::At(const size_t x, const size_t y) const {
	/*if (x >= BOARD_WIDTH) {
		throw std::runtime_error(std::format("Bad Board::At call: argument x = {} was not within BOARD_WIDTH = {}.", 
			x, BOARD_WIDTH));
	}
	if (y >= BOARD_HEIGHT) {
		throw std::runtime_error(std::format("Bad Board::At call: argument y = {} was not within BOARD_HEIGHT = {}.",
			y, BOARD_HEIGHT));
	}*/
	return GetState(cells, y * BOARD_WIDTH + x);
}

CellState Board::At(const std::pair<size_t, size_t> pos) const
{
	return At(pos.first, pos.second);
}

Board Board::Play(const size_t x, const size_t y, const bool blue) const {
	if (x >= BOARD_WIDTH) {
		throw std::runtime_error(std::format("Bad Board::Play call: argument x = {} was not within BOARD_WIDTH = {}.",
			x, BOARD_WIDTH));
	}
	if (y >= BOARD_HEIGHT) {
		throw std::runtime_error(std::format("Bad Board::Play call: argument y = {} was not within BOARD_HEIGHT = {}.",
			y, BOARD_HEIGHT));
	}

	Board b = *this;
	SetCrumb(b.cells, y * BOARD_WIDTH + x, blue ? CellState::BLUE : CellState::RED);
	return b;
}

Board Board::Play(const std::pair<size_t, size_t> pos, const bool blue) const {
	return Play(pos.first, pos.second, blue);
}

Board Board::Play(const EmptyIterator& cell, const bool blue) const {
	if (cell == EmptyEnd()) {
		throw std::runtime_error("Bad Board::Play call: argument cell was invalid (== EmptyEnd()).");
	}

	Board b = *this;
	SetCrumb(b.cells, cell.cursor, blue ? CellState::BLUE : CellState::RED);
	return b;
}

Board Board::Play(const InRangeIterator& cell, const bool blue) const {
	if (cell == InRangeEnd()) {
		throw std::runtime_error("Bad Board::Play call: argument cell was invalid (== EmptyEnd()).");
	}

	Board b = *this;
	SetCrumb(b.cells, cell.cursor, blue ? CellState::BLUE : CellState::RED);
	return b;
}

Board::EmptyIterator Board::Selected(const std::pair<double, double> windowPos) const {
	const auto intX = static_cast<int>(windowPos.first) / CELL_PIXEL_WIDTH;
	const auto intY = static_cast<int>(windowPos.second) / CELL_PIXEL_WIDTH;

	if (intX < 0 || intX >= BOARD_WIDTH ||
		intY < 0 || intY >= BOARD_HEIGHT)
		return EmptyEnd();

	// We know that intX and intY are positive, so they can be casted to size_t
	const auto boardPos = std::make_pair(static_cast<size_t>(intX), static_cast<size_t>(intY));

	for (auto it = EmptyBegin(); it != EmptyEnd(); ++it) {
		if (it.Position() == boardPos) {
			return it;
		}
	}

	return EmptyEnd();
}


// Iterator stuff

CellState Board::AllIterator::operator*() const {
	const auto [x, y] = Position();
	return board.At(x, y);
}

Board::AllIterator& Board::AllIterator::operator++() {
	if (*this == board.AllEnd()) {
		throw std::runtime_error("Cannot iterate past end.");
	}
	++cursor;
	return *this;
}

Board::AllIterator Board::AllIterator::operator++(int) {
	AllIterator old = *this;
	++*this;
	return old;
}

bool Board::AllIterator::operator==(const Board::AllIterator& other) const {
	return &board == &other.board && cursor == other.cursor;
}

std::pair<size_t, size_t> Board::AllIterator::Position() const {
	return std::make_pair(cursor % BOARD_WIDTH, cursor / BOARD_WIDTH);
}

Board::AllIterator::AllIterator(const size_t cursor, const Board& board) : cursor(cursor), board(board) {};

CellState Board::EmptyIterator::operator*() const {
	const auto [x, y] = Position();
	return board.At(x, y);
}

Board::EmptyIterator& Board::EmptyIterator::operator++() {
	if (*this == board.EmptyEnd()) {
		throw std::runtime_error("Cannot iterate past end.");
	}
	do {
		++cursor;
	} while (cursor < BOARD_WIDTH * BOARD_HEIGHT && **this != CellState::EMPTY);
	return *this;
}

Board::EmptyIterator Board::EmptyIterator::operator++(int) {
	EmptyIterator old = *this;
	++*this;
	return old;
}

bool Board::EmptyIterator::operator==(const Board::EmptyIterator& other) const {
	return &board == &other.board && cursor == other.cursor;
}

std::pair<size_t, size_t> Board::EmptyIterator::Position() const {
	return std::make_pair(cursor % BOARD_WIDTH, cursor / BOARD_WIDTH);
}

Board::EmptyIterator::EmptyIterator(const size_t cursor, const Board& board) : cursor(cursor), board(board) {};

CellState Board::InRangeIterator::operator*() const {
	const auto [x, y] = Position();
	return board->At(x, y);
}

Board::InRangeIterator& Board::InRangeIterator::operator++() {
	if (*this == board->InRangeEnd()) {
		throw std::runtime_error("Cannot iterate past end.");
	}
	do {
		++cursor;
	} while (cursor < BOARD_WIDTH * BOARD_HEIGHT && !Valid());
	return *this;
}

Board::InRangeIterator Board::InRangeIterator::operator++(int) {
	InRangeIterator old = *this;
	++*this;
	return old;
}

bool Board::InRangeIterator::operator==(const Board::InRangeIterator& other) const {
	return board == other.board && cursor == other.cursor;
}

std::pair<size_t, size_t> Board::InRangeIterator::Position() const {
	return std::make_pair(cursor % BOARD_WIDTH, cursor / BOARD_WIDTH);
}

Board::InRangeIterator::InRangeIterator(const InRangeIterator& other) : cursor(other.cursor), board(other.board) {}

Board::InRangeIterator& Board::InRangeIterator::operator=(const InRangeIterator& other) {
	cursor = other.cursor;
	board = other.board;
	return *this;
}

bool Board::InRangeIterator::Valid() const {
	const auto [x, y] = Position();

	if (board->At(x, y) != CellState::EMPTY) {
		return false;
	}

	for (size_t j = ((y < RANGE) ? 0 : y - RANGE); j < ((y + RANGE + 1 < BOARD_HEIGHT) ? y + RANGE + 1 : BOARD_HEIGHT); ++j) {
		for (size_t i = ((x < RANGE) ? 0 : x - RANGE); i < ((x + RANGE + 1 < BOARD_WIDTH) ? x + RANGE + 1 : BOARD_WIDTH); ++i) {
			if (!(i == x && j == y) && board->At(i, j) != CellState::EMPTY) {
				return true;
			}
		}
	}

	return false;
}

Board::InRangeIterator::InRangeIterator(const size_t cursor, const Board* board) : cursor(cursor), board(board) {};

int8_t Board::FivesIterator::operator*() const {
	const int headX = static_cast<int>(head % BOARD_WIDTH);
	const int headY = static_cast<int>(head / BOARD_WIDTH);
	const int tailX = static_cast<int>(tail % BOARD_WIDTH);
	const int tailY = static_cast<int>(tail / BOARD_WIDTH);

	int8_t count{};
	for (int i = 0; i < 5; ++i) {
		auto state = board.At(static_cast<size_t>(headX + i * ((tailX - headX) / 4)), 
			static_cast<size_t>(headY + i * ((tailY - headY) / 4)));
		if (state == CellState::BLUE) {
			if (count > 0) return 0;
			--count;
		}
		else if (state == CellState::RED) {
			if (count < 0) return 0;
			++count;
		}
	}

	return count;
}

Board::FivesIterator& Board::FivesIterator::operator++() {
	if (*this == board.FivesEnd()) {
		throw std::runtime_error("Cannot iterate past end.");
	}
	
	const auto headX = head % BOARD_WIDTH;
	const auto headY = head / BOARD_WIDTH;
	const auto tailX = tail % BOARD_WIDTH;
	const auto tailY = tail / BOARD_WIDTH;

	static size_t horizontal{};
	static size_t vertical{};
	static size_t diagonalSE{};
	static size_t diagonalSW{};

	// Horizontal
	if (headY == tailY) {
		++horizontal;
		// Can we move right?
		if (tailX + 1 < BOARD_WIDTH) {
			++head;
			++tail;
		}
		// Can we move down?
		else if (tailY + 1 < BOARD_HEIGHT) {
			head += 5;
			tail += 5;
		}
		// Next Orientation
		else {
			head = 0;
			tail = 4 * BOARD_WIDTH;
		}
	}
	// Vertical
	else if (headX == tailX) {
		++vertical;
		// Can we move right or down?
		if (tail + 1 < BOARD_WIDTH * BOARD_HEIGHT) {
			++head;
			++tail;
		}
		// Next Orientation
		else {
			head = 0;
			tail = 4 * BOARD_WIDTH + 4;
		}
	}
	// Diagonal Southeast
	else if (headX < tailX) {
		++diagonalSE;
		// Can we move right?
		if (tailX + 1 < BOARD_WIDTH) {
			++head;
			++tail;
		}
		// Can we move down?
		else if (tailY + 1 < BOARD_HEIGHT) {
			head += 5;
			tail += 5;
		}
		// Next Orientation
		else {
			head = 4;
			tail = 4 * BOARD_WIDTH;
		}
	}
	// Diagonal Southwest (only possibility)
	else {
		++diagonalSW;
		if (headX + 1 < BOARD_WIDTH) {
			++head;
			++tail;
		}
		// Can we move down?
		else if (tailY + 1 < BOARD_HEIGHT) {
			head += 5;
			tail += 5;
		}
		// End
		else {
			head = tail = BOARD_WIDTH * BOARD_HEIGHT;
		}
	}
	return *this;
}

Board::FivesIterator Board::FivesIterator::operator++(int) {
	FivesIterator it = *this;
	++*this;
	return it;
}

bool Board::FivesIterator::operator==(const Board::FivesIterator& rhs) const {
	return (head == rhs.head && tail == rhs.tail);
}

std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t>> Board::FivesIterator::Positions() const {
	return std::make_pair(std::make_pair(head % BOARD_WIDTH, head / BOARD_WIDTH), std::make_pair(tail % BOARD_WIDTH, tail / BOARD_WIDTH));
}

Board::FivesIterator::FivesIterator(const size_t head, const size_t tail, const Board& board) :
	head(head), tail(tail), board(board) {}

Board::AllIterator Board::AllBegin() const {
	return AllIterator{ 0, *this };
}

Board::AllIterator Board::AllEnd() const {
	return AllIterator{ BOARD_WIDTH * BOARD_HEIGHT, *this };
}

Board::EmptyIterator Board::EmptyBegin() const {
	EmptyIterator it{ 0, *this };
	if (*it != CellState::EMPTY) {
		++it;
	}
	return it;
}

Board::EmptyIterator Board::EmptyEnd() const {
	return EmptyIterator{ BOARD_WIDTH * BOARD_HEIGHT, *this };
}

Board::InRangeIterator Board::InRangeBegin() const {
	InRangeIterator it(0, this);
	if (!it.Valid()) {
		++it;
	}
	return it;
}

Board::InRangeIterator Board::InRangeEnd() const {
	return InRangeIterator{ BOARD_WIDTH * BOARD_HEIGHT, this };
}



Board::FivesIterator Board::FivesEnd() const {
	return FivesIterator{ BOARD_WIDTH * BOARD_HEIGHT, BOARD_WIDTH * BOARD_HEIGHT, *this };
}


