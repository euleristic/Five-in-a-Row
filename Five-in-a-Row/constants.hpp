// This header provides application wide constants as constexpr, and may partly be used for configuration
// The static_asserts alongside the configurations confine the configuration

#pragma once

#include <limits>

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
	// PLAYER_COLOR
	// COMPUTER_COLOR

	// Window title

	constexpr const char* APPLICATION_NAME = "Five in a Row - ";
	constexpr const char* PLAYER_TURN_SUFFIX = "Your turn!";
	constexpr const char* COMPUTER_TURN_SUFFIX = "Thinking...";
	constexpr const char* PLAYER_WIN_SUFFIX = "You won!";
	constexpr const char* COMPUTER_WIN_SUFFIX = "You lost!";
	constexpr const char* DRAW_SUFFIX = "Draw!";

	// AI related
	constexpr size_t RANGE = 1;							static_assert(RANGE > 0);
	constexpr size_t PLY_LOOK_AHEAD = 4;				static_assert(PLY_LOOK_AHEAD > 0);
	constexpr float SCORE_MAP[5]{
		1.0f * 1.0f * 1.0f,
		2.0f * 2.0f * 2.0f,
		3.0f * 3.0f * 3.0f,
		4.0f * 4.0f * 4.0f,
		std::numeric_limits<float>::infinity()
	};
	
	// CONSEQUENTIAL CONSTANTS (don't play with these)

	constexpr int WINDOW_WIDTH = CELL_PIXEL_WIDTH * BOARD_WIDTH;
	constexpr int WINDOW_HEIGHT = CELL_PIXEL_WIDTH * BOARD_HEIGHT;


	// UNIVERSAL CONSTANTS

	constexpr size_t GOAL_FUNCTION_THREAD_COUNT = 4;

	constexpr float NORMAL_SQUARE_VERTEX_BUFFER[12] = {
		-1.0f, 1.0f,
		1.0f, 1.0f,
		-1.0f, -1.0f,
		1.0f, 1.0f,
		1.0f, -1.0f,
		-1.0f, -1.0f
	};

}