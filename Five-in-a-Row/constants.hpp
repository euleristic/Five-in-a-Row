// This header provides application wide constants as constexpr, and may partly be used for configuration
// The static_asserts alongside the configurations confine the configuration

#pragma once

#include <limits>
#include <vector>
#include <array>

// First, some types

namespace Constants {

	// CONFIGURATION CONSTANTS (play with these)

	// General

	constexpr bool PLAYER_FIRST = true;
	constexpr size_t BOARD_WIDTH = 15;			static_assert(BOARD_WIDTH >= 15);
	constexpr size_t BOARD_HEIGHT = 15;			static_assert(BOARD_HEIGHT >= 15);
	constexpr int CELL_PIXEL_WIDTH = 50;		static_assert(CELL_PIXEL_WIDTH > 0);
	constexpr float CELL_WIDTH_FACTOR = 0.95f;	static_assert(0.0f < CELL_WIDTH_FACTOR && CELL_WIDTH_FACTOR < 1.0f);

	// How to draw a piece

	constexpr float PIECE_WIDTH_FACTOR = 0.9f;	static_assert(0.0f < PIECE_WIDTH_FACTOR && PIECE_WIDTH_FACTOR < 1.0f);
	constexpr float PIECE_INNER_FACTOR = 0.8f;	static_assert(0.0f < PIECE_INNER_FACTOR && PIECE_INNER_FACTOR < 1.0f);
	constexpr float OUTER_COLOR_FACTOR = 0.7f;	static_assert(0.0f < OUTER_COLOR_FACTOR && OUTER_COLOR_FACTOR < 1.0f);

	// Window title

	constexpr const char* APPLICATION_NAME = "Five in a Row - ";
	constexpr const char* PLAYER_TURN_SUFFIX = "Your turn!";
	constexpr const char* COMPUTER_TURN_SUFFIX = "Thinking...";
	constexpr const char* PLAYER_WIN_SUFFIX = "You won!";
	constexpr const char* COMPUTER_WIN_SUFFIX = "You lost!";
	constexpr const char* DRAW_SUFFIX = "Draw!";

	// AI related
	
	// How far (Chebyshev distance) away from any other piece the AI will consider playing
	constexpr size_t RANGE = 1;						static_assert(RANGE > 0);

	// The depth of the minimax search
	constexpr size_t PLY_LOOK_AHEAD = 5;			static_assert(PLY_LOOK_AHEAD > 0);

	// For how many depths (from the root ply) the minimax will sort it's children based 
	// on their immediate goal function score
	constexpr size_t SORTING_DEPTH = 2;

	// SCORE_MAP[n - 1] is the term to add to the goal function for a number n pieces in a "five"
	constexpr float SCORE_MAP[]{
		1.0f * 1.0f * 1.0f,
		2.0f * 2.0f * 2.0f,
		3.0f * 3.0f * 3.0f,
		4.0f * 4.0f * 4.0f,
		std::numeric_limits<float>::infinity()
	};
	


	// CONSEQUENTIAL CONSTANTS (don't play with these)

	constexpr size_t BOARD_SIZE = BOARD_WIDTH * BOARD_HEIGHT;
	constexpr int WINDOW_WIDTH = CELL_PIXEL_WIDTH * BOARD_WIDTH;
	constexpr int WINDOW_HEIGHT = CELL_PIXEL_WIDTH * BOARD_HEIGHT;

	// Generate "five" "root" arrays at compile time 

	consteval auto HORIZONTAL_FIVES_GENERATOR() {
		std::array<size_t, (BOARD_WIDTH - 4) * BOARD_HEIGHT> arr;
		size_t index = 0;
		for (size_t j = 0; j < BOARD_HEIGHT; ++j) {
			for (size_t i = 0; i < BOARD_WIDTH - 4; ++i, ++index) {
				arr[index] = (j * BOARD_WIDTH + i);
			}
		}
		return arr;
	}

	consteval auto VERTICAL_FIVES_GENERATOR() {
		std::array<size_t, BOARD_WIDTH * (BOARD_HEIGHT - 4)> arr;
		for (size_t i = 0; i < arr.size(); ++i) {
			arr[i] = i;
		}
		return arr;
	}

	consteval auto SOUTHEAST_FIVES_GENERATOR() {
		std::array<size_t, (BOARD_WIDTH - 4) * (BOARD_HEIGHT - 4)> arr;
		size_t index = 0;
		for (size_t j = 0; j < BOARD_HEIGHT - 4; ++j) {
			for (size_t i = 0; i < BOARD_WIDTH - 4; ++i, ++index) {
				arr[index] = (j * BOARD_WIDTH + i);
			}
		}
		return arr;
	}

	consteval auto SOUTHWEST_FIVES_GENERATOR() {
		std::array<size_t, (BOARD_WIDTH - 4) * (BOARD_HEIGHT - 4)> arr;
		size_t index = 0;
		for (size_t j = 0; j < BOARD_HEIGHT - 4; ++j) {
			for (size_t i = 4; i < BOARD_WIDTH; ++i, ++index) {
				arr[index] = (j * BOARD_WIDTH + i);
			}
		}
		return arr;
	}

	constexpr auto HORIZONTAL_FIVES_ROOTS = HORIZONTAL_FIVES_GENERATOR();
	constexpr auto VERTICAL_FIVES_ROOTS = VERTICAL_FIVES_GENERATOR();
	constexpr auto SOUTHEAST_FIVES_ROOTS = SOUTHEAST_FIVES_GENERATOR();
	constexpr auto SOUTHWEST_FIVES_ROOTS = SOUTHWEST_FIVES_GENERATOR();

	// UNIVERSAL CONSTANTS

	constexpr float NORMAL_SQUARE_VERTEX_BUFFER[12] = {
		-1.0f, 1.0f,
		1.0f, 1.0f,
		-1.0f, -1.0f,
		1.0f, 1.0f,
		1.0f, -1.0f,
		-1.0f, -1.0f
	};

}