// This header contains the declarations of the Board class and cell state,
// which encapsulate a board state in a memory-efficient manner

#pragma once

#include "constants.hpp"

#include <string_view>
#include <bitset>
#include <utility>

enum class CellState { EMPTY, BLUE, RED };

class Board {
public:
	Board() noexcept;

	// For testing. '*' is EMPTY, 'B' is BLUE and 'R' is RED.
	Board(const std::string_view input);

	bool BlueWin() const;
	bool RedWin() const;

	CellState At(const size_t x, const size_t y) const;
	CellState At(const std::pair<size_t, size_t> pos) const;
	Board Play(const size_t x, const size_t y, const bool blue) const;
	Board Play(const std::pair<size_t, size_t> pos, const bool blue) const;

	Board Reset(const size_t x, const size_t y) const;
	Board Reset(const std::pair<size_t, size_t> pos) const;

	class EmptyIterator;
	class InRangeIterator;

	Board Play(const EmptyIterator& cell, const bool blue) const;
	Board Play(const InRangeIterator& cell, const bool blue) const;
	EmptyIterator Selected(const std::pair<double, double> windowPosition) const;

	// Iterators (all forward)

	class AllIterator {
	public:
		CellState operator*() const;
		AllIterator& operator++();
		AllIterator operator++(int);
		bool operator==(const AllIterator& rhs) const;
		std::pair<size_t, size_t> Position() const;

	private:
		friend class Board;
		AllIterator(const size_t cursor, const Board& board);
		size_t cursor;
		const Board& board;
	};

	class EmptyIterator {
	public:
		CellState operator*() const;
		EmptyIterator& operator++();
		EmptyIterator operator++(int);
		bool operator==(const EmptyIterator& rhs) const;
		std::pair<size_t, size_t> Position() const;
	private:
		friend class Board;
		EmptyIterator(const size_t cursor, const Board& board);
		size_t cursor;
		const Board& board;
	};

	class InRangeIterator {
	public:
		CellState operator*() const;
		InRangeIterator& operator++();
		InRangeIterator operator++(int);
		bool operator==(const InRangeIterator& rhs) const;
		std::pair<size_t, size_t> Position() const;
		InRangeIterator(const InRangeIterator& other);
		InRangeIterator& operator=(const InRangeIterator& other);
	private:
		bool Valid() const;
		friend class Board;
		InRangeIterator(const size_t cursor, const Board* board);
		size_t cursor;
		const Board* board;
	};

	class HorizontalFivesIterator {
	public:
		// 0 if empty or containing both colors. If positive, how many red pieces the five contains, if negative the equivalent for blue.
		int8_t operator*() const;
		HorizontalFivesIterator& operator++();
		HorizontalFivesIterator operator++(int);
		bool operator==(const HorizontalFivesIterator& rhs) const;
	private:
		size_t cursor;
		friend class Board;
		HorizontalFivesIterator(const size_t cursor, const Board* board);
		const Board* board;
	};

	class VerticalFivesIterator {
	public:
		int8_t operator*() const;
		VerticalFivesIterator& operator++();
		VerticalFivesIterator operator++(int);
		bool operator==(const VerticalFivesIterator& rhs) const;
	private:
		size_t cursor;
		friend class Board;
		VerticalFivesIterator(const size_t cursor, const Board* board);
		const Board* board;
	};

	class SoutheastFivesIterator {
	public:
		// 0 if empty or containing both colors. If positive, how many red pieces the five contains, if negative the equivalent for blue.
		int8_t operator*() const;
		SoutheastFivesIterator& operator++();
		SoutheastFivesIterator operator++(int);
		bool operator==(const SoutheastFivesIterator& rhs) const;
	private:
		size_t cursor;
		friend class Board;
		SoutheastFivesIterator(const size_t cursor, const Board* board);
		const Board* board;
	};

	class SouthwestFivesIterator {
	public:
		// 0 if empty or containing both colors. If positive, how many red pieces the five contains, if negative the equivalent for blue.
		int8_t operator*() const;
		SouthwestFivesIterator& operator++();
		SouthwestFivesIterator operator++(int);
		bool operator==(const SouthwestFivesIterator& rhs) const;
	private:
		size_t cursor;
		friend class Board;
		SouthwestFivesIterator(const size_t cursor, const Board* board);
		const Board* board;
	};

	AllIterator AllBegin() const;
	AllIterator AllEnd() const;
	EmptyIterator EmptyBegin() const;
	EmptyIterator EmptyEnd() const;
	InRangeIterator InRangeBegin() const;
	InRangeIterator InRangeEnd() const;

	HorizontalFivesIterator HorizontalFivesBegin() const;
	HorizontalFivesIterator HorizontalFivesEnd() const;
	VerticalFivesIterator VerticalFivesBegin() const;
	VerticalFivesIterator VerticalFivesEnd() const;
	SoutheastFivesIterator SoutheastFivesBegin() const;
	SoutheastFivesIterator SoutheastFivesEnd() const;
	SouthwestFivesIterator SouthwestFivesBegin() const;
	SouthwestFivesIterator SouthwestFivesEnd() const;


private:
	// The underlying board state representation 
	std::bitset<2 * Constants::BOARD_WIDTH * Constants::BOARD_HEIGHT> cells;	
};

template <size_t pos>
Board::FivesIterator Board::FivesBegin() const {
	static_assert(pos < 4);
	if constexpr (pos == 0)
		return FivesIterator{ 0, 4, *this };
	if constexpr (pos == 1)
		return FivesIterator{ 0, 4 * Constants::BOARD_WIDTH, *this };
	if constexpr (pos == 2)
		return FivesIterator{ 0, 4 * Constants::BOARD_WIDTH + 4, *this };
	if constexpr (pos == 3)
		return FivesIterator{ 4, 4 * Constants::BOARD_WIDTH, *this };
}

//static_assert(std::input_iterator<Board::AllIterator>);
//static_assert(std::input_iterator<Board::EmptyIterator>);
//static_assert(std::input_iterator<Board::InRangeIterator>);