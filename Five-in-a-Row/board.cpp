#include "board.hpp"

#include <stdexcept>
#include <format>
#include <vector>
#include <optional>
#include <algorithm>

using namespace Constants;

// Utilities

// An alias for the bitset, for brevity
using RepType = std::bitset<2 * BOARD_SIZE>;

// Sets one cell on the bitset representation
void SetCrumb(RepType& of, const size_t at, const CellState to) {
	of.set(at * 2 + 1, to != CellState::EMPTY);
	of.set(at * 2, to == CellState::RED);
}
// Supposedly, a crumb is the base-4 equivalent of base-2's bit, base-3's trit or base-256's byte: 
// https://en.wikipedia.org/wiki/Quaternary_numeral_system#Relation_to_binary_and_hexadecimal

// Get the cellstate from the bitset representation
CellState GetState(const RepType& of, const size_t at) {
	if (!of.test(at * 2 + 1))
		return CellState::EMPTY;
	return of.test(at * 2) ? CellState::RED : CellState::BLUE;
}

Board::Board() : cells{} {}

Board::Board(const std::string_view input) {
	if (input.length() != BOARD_SIZE) {
		throw std::runtime_error("Could not construct Board : bad input length");
	}
	for (size_t i = 0; i < BOARD_SIZE; ++i) {
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

CellState Board::At(const size_t pos) const {
#ifndef NDEBUG
	if (pos >= BOARD_SIZE) {
		throw std::runtime_error(std::format("Bad Board::At call: argument pos = {} was not within BOARD_SIZE = {}.",
			pos, BOARD_SIZE));
	}
#endif // NDEBUG

	return GetState(cells, pos);
}

CellState Board::At(const size_t x, const size_t y) const {
	return At(y * BOARD_WIDTH + x);
}

CellState Board::At(const std::pair<size_t, size_t> pos) const {
	return At(pos.second * BOARD_WIDTH + pos.first);
}

Board Board::Play(const size_t pos, const bool blue) const {
#ifndef NDEBUG
	if (pos >= BOARD_SIZE) {
		throw std::runtime_error(std::format("Bad Board::Play call: argument pos = {} was not within BOARD_SIZE = {}.",
			pos, BOARD_SIZE));
	}
#endif // NDEBUG
	Board b = *this;
	SetCrumb(b.cells, pos, blue ? CellState::BLUE : CellState::RED);
	return b;
}

Board Board::Play(const size_t x, const size_t y, const bool blue) const {
	return Play(y * BOARD_WIDTH + x, blue);
}

Board Board::Play(const std::pair<size_t, size_t> pos, const bool blue) const {
	return Play(pos.second * BOARD_WIDTH + pos.first, blue);
}

Board Board::Reset(const size_t pos) const {
#ifndef NDEBUG
	if (pos >= BOARD_SIZE) {
		throw std::runtime_error(std::format("Bad Board::Reset call: argument pos = {} was not within BOARD_SIZE = {}.",
			pos, BOARD_SIZE));
	}
#endif // NDEBUG

	Board b = *this;
	SetCrumb(b.cells, pos, CellState::EMPTY);
	return b;
}

Board Board::Reset(const size_t x, const size_t y) const {
	return Reset(y * BOARD_WIDTH + x);
}

Board Board::Reset(const std::pair<size_t, size_t> pos) const {
	return Reset(pos.second * BOARD_WIDTH + pos.first);
}

std::optional<size_t> Board::Selected(const std::pair<double, double> cursorPos) const {
	// Transform the cursor position from window space to board space
	const auto x = static_cast<int>(cursorPos.first) / CELL_PIXEL_WIDTH;
	const auto y = static_cast<int>(cursorPos.second) / CELL_PIXEL_WIDTH;

	// Is the board space position a cell on the board?
	if (x < 0 || static_cast<size_t>(x) >= BOARD_WIDTH ||
		y < 0 || static_cast<size_t>(y) >= BOARD_HEIGHT)
		return {};

	// Is the cell empty?
	for (size_t j = 0; j < BOARD_HEIGHT; ++j) {
		for (size_t i = 0; i < BOARD_WIDTH; ++i) {
			if (At(i, j) == CellState::EMPTY &&
				i == static_cast<size_t>(x) &&
				j == static_cast<size_t>(y)) {
				return j * BOARD_WIDTH + i;
			}
		}
	}

	// The cell is occupied
	return {};
}

bool Board::InRange(const size_t pos) const {
	// If the cell is not empty, it is not considered "in range"
	if (At(pos) != CellState::EMPTY) {
		return false;
	}

	const auto x = pos % BOARD_WIDTH;
	const auto y = pos / BOARD_WIDTH;

	// Look for a piece in the intersection of the "in range" square of pos and the board
	for (size_t j = ((y < RANGE) ? 0 : y - RANGE); j < ((y + RANGE + 1 < BOARD_HEIGHT) ? y + RANGE + 1 : BOARD_HEIGHT); ++j) {
		for (size_t i = ((x < RANGE) ? 0 : x - RANGE); i < ((x + RANGE + 1 < BOARD_WIDTH) ? x + RANGE + 1 : BOARD_WIDTH); ++i) {
			if (!(i == x && j == y) && At(i, j) != CellState::EMPTY) {
				return true;
			}
		}
	}

	// There's no nearby piece, pos is out of range
	return false;
}

std::vector<size_t> Board::InRangePlies() const {
	// If the board the board is empty, a middle cell is chosen
	if (Empty()) {
		return { BOARD_SIZE / 2 + ((BOARD_HEIGHT % 2) ? 0 : BOARD_WIDTH / 2) };
	}

	// Simply iterate the board and push back "in range" positions into the returned vector
	std::vector<size_t> v;
	for (size_t i = 0; i < BOARD_SIZE; ++i) {
		if (InRange(i)) {
			v.push_back(i);
		}
	}
	return v;
}


template <>
int8_t Board::CountFive<FivesOrientation::HORIZONTAL>(const size_t root) const {
#ifndef NDEBUG
	if (std::none_of(HORIZONTAL_FIVES_ROOTS.begin(), HORIZONTAL_FIVES_ROOTS.end(), [root](size_t i) {
		return i == root;
	})) {
		throw std::out_of_range("The \"five\" does not exist");
	}
#endif // NDEBUG
	int8_t count{};
	for (size_t i = 0; i < 5; ++i) {
		auto state = GetState(cells, root + i); // Right
		if ((state == CellState::BLUE && count > 0) ||
			(state == CellState::RED && count < 0)) {
			return 0;
		}
		count += (state == CellState::RED);
		count -= (state == CellState::BLUE);
	}
	return count;
}

template <>
int8_t Board::CountFive<FivesOrientation::VERTICAL>(const size_t root) const {
#ifndef NDEBUG
	if (std::none_of(VERTICAL_FIVES_ROOTS.begin(), VERTICAL_FIVES_ROOTS.end(), [root](size_t index) {
		return index == root;
	})) {
		throw std::out_of_range("The \"five\" does not exist");
	}
#endif // NDEBUG
	int8_t count{};
	for (size_t i = 0; i < 5; ++i) {
		auto state = GetState(cells, root + i * BOARD_WIDTH); // Down
		if ((state == CellState::BLUE && count > 0) ||
			(state == CellState::RED && count < 0)) {
			return 0;
		}
		count += (state == CellState::RED);
		count -= (state == CellState::BLUE);
	}
	return count;
}

template <>
int8_t Board::CountFive<FivesOrientation::SOUTHEAST>(const size_t root) const {
#ifndef NDEBUG
	if (std::none_of(SOUTHEAST_FIVES_ROOTS.begin(), SOUTHEAST_FIVES_ROOTS.end(), [root](size_t index) {
		return index == root;
	})) {
		throw std::out_of_range("The \"five\" does not exist");
	}
#endif // NDEBUG
	int8_t count{};
	for (size_t i = 0; i < 5; ++i) {
		auto state = GetState(cells, root + i * BOARD_WIDTH + i); // Down and right
		if ((state == CellState::BLUE && count > 0) ||
			(state == CellState::RED && count < 0)) {
			return 0;
		}
		count += (state == CellState::RED);
		count -= (state == CellState::BLUE);
	}
	return count;
}

template <>
int8_t Board::CountFive<FivesOrientation::SOUTHWEST>(const size_t root) const {
#ifndef NDEBUG
	if (std::none_of(SOUTHWEST_FIVES_ROOTS.begin(), SOUTHWEST_FIVES_ROOTS.end(), [root](size_t index) {
		return index == root;
	})) {
		throw std::out_of_range("The \"five\" does not exist");
	}
#endif // NDEBUG
	int8_t count{};
	for (size_t i = 0; i < 5; ++i) {
		auto state = GetState(cells, root + i * BOARD_WIDTH - i); // Down and left
		if ((state == CellState::BLUE && count > 0) ||
			(state == CellState::RED && count < 0)) {
			return 0;
		}
		count += (state == CellState::RED);
		count -= (state == CellState::BLUE);
	}
	return count;
}

bool Board::BlueWin() const {
	return std::any_of(HORIZONTAL_FIVES_ROOTS.begin(), HORIZONTAL_FIVES_ROOTS.end(),
		[&](const size_t root) { return CountFive<FivesOrientation::HORIZONTAL>(root) == -5; })
		|| std::any_of(VERTICAL_FIVES_ROOTS.begin(), VERTICAL_FIVES_ROOTS.end(),
			[&](const size_t root) { return CountFive<FivesOrientation::VERTICAL>(root) == -5; })
		|| std::any_of(SOUTHEAST_FIVES_ROOTS.begin(), SOUTHEAST_FIVES_ROOTS.end(),
			[&](const size_t root) { return CountFive<FivesOrientation::SOUTHEAST>(root) == -5; })
		|| std::any_of(SOUTHWEST_FIVES_ROOTS.begin(), SOUTHWEST_FIVES_ROOTS.end(),
			[&](const size_t root) { return CountFive<FivesOrientation::SOUTHWEST>(root) == -5; });
}

bool Board::RedWin() const {
	return std::any_of(HORIZONTAL_FIVES_ROOTS.begin(), HORIZONTAL_FIVES_ROOTS.end(),
		[&](const size_t root) { return CountFive<FivesOrientation::HORIZONTAL>(root) == 5; })
		|| std::any_of(VERTICAL_FIVES_ROOTS.begin(), VERTICAL_FIVES_ROOTS.end(),
			[&](const size_t root) { return CountFive<FivesOrientation::VERTICAL>(root) == 5; })
		|| std::any_of(SOUTHEAST_FIVES_ROOTS.begin(), SOUTHEAST_FIVES_ROOTS.end(),
			[&](const size_t root) { return CountFive<FivesOrientation::SOUTHEAST>(root) == 5; })
		|| std::any_of(SOUTHWEST_FIVES_ROOTS.begin(), SOUTHWEST_FIVES_ROOTS.end(),
			[&](const size_t root) { return CountFive<FivesOrientation::SOUTHWEST>(root) == 5; });
}

bool Board::Empty() const {
	return !cells.any();
}

bool Board::Full() const {
	for (size_t i = 0; i < BOARD_SIZE; ++i) {
		if (At(i) == CellState::EMPTY) return false;
	}
	return true;
}