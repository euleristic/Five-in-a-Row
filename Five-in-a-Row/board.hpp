// This header contains the declarations of the Board class and cell state,
// which encapsulate a board state in a memory-efficient manner
// It is also entirely const and therefore threadsafe! Yay for value-semantics

#pragma once

#include "constants.hpp"

#include <string_view>
#include <bitset>
#include <utility>
#include <optional>

// The state of a cell
enum class CellState { EMPTY, BLUE, RED };

// The orientations of possible five-in-a-rows
enum class FivesOrientation { HORIZONTAL, VERTICAL, SOUTHEAST, SOUTHWEST };

class Board {
public:
	// Creates an empty board.
	Board();

	// For testing. '*' is EMPTY, 'B' is BLUE and 'R' is RED.
	Board(const std::string_view input);

	// Returns the state of the cell at position pos
	CellState At(const size_t pos) const;
	// Returns the state of the cell at position (x,y)
	CellState At(const size_t x, const size_t y) const;
	// Returns the state of the cell at position pos
	CellState At(const std::pair<size_t, size_t> pos) const;

	// Returns the board resulting from playing a piece at pos
	Board Play(const size_t pos, const bool blue) const;
	// Returns the board resulting from playing a piece at (x,y)
	Board Play(const size_t x, const size_t y, const bool blue) const;
	// Returns the board resulting from playing a piece at pos
	Board Play(const std::pair<size_t, size_t> pos, const bool blue) const;

	// Returns the board resulting from removing a piece at pos
	Board Reset(const size_t pos) const;
	// Returns the board resulting from removing a piece at (x,y)
	Board Reset(const size_t x, const size_t y) const;
	// Returns the board resulting from removing a piece at pos
	Board Reset(const std::pair<size_t, size_t> pos) const;

	// Returns the position of the empty cell selected by cursorPosition, if existent
	std::optional<size_t> Selected(const std::pair<double, double> cursorPosition) const;

	// Returns whether the the cell at pos is "in range"
	// I.e. the cell is within Constants::RANGE (Chebyshev distance) of any piece and is empty
	bool InRange(const size_t pos) const;
	// Returns a vector containing all current "in range" positions on the board
	std::vector<size_t> InRangePlies() const;

	template <FivesOrientation orientation>
	// Counts pieces along the orientation, from root
	// If positive, the count of red pieces; if negative, the count of blue pieces;
	// if zero, empty or or containing both
	int8_t CountFive(const size_t root) const;

	// Does blue have five-in-a-row?
	bool BlueWin() const;
	// Does red have five-in-a-row?
	bool RedWin() const;

	// Is the board empty?
	bool Empty() const;
	// Is the board full?
	bool Full() const;
private:
	// The underlying board state representation
	std::bitset<2 * Constants::BOARD_SIZE> cells;
};
