#include "scopedLibrary.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include "board.hpp"
#include "minimax.hpp"
#include "goalFunction.hpp"

#include <iostream>
#include <vector>

auto main() -> int {
	try {
		ScopedLibrary lib;
		Window window;
		Renderer renderer;

		Board board;

		constexpr Minimax<Constants::PLY_LOOK_AHEAD, true, GoalFunction, true, true> computer;

		bool playerTurn = Constants::PLAYER_FIRST;
		bool gameOver = false;

		//if 

		while (!window.ShouldClose()) {

			// Game logic
			if (gameOver) {
				if (window.ResetPressed()) {
					board = Board{};
					gameOver = false;
					playerTurn = Constants::PLAYER_FIRST;
					window.SetTitle((std::string(Constants::APPLICATION_NAME) + (playerTurn ?
						Constants::PLAYER_TURN_SUFFIX : Constants::COMPUTER_TURN_SUFFIX)).c_str());
				}
			}
			else {
				if (playerTurn) {
					if (const auto selected = board.Selected(window.CursorPosition()); selected != board.EmptyEnd() && window.Clicked()) {
						board = board.Play(selected, true);
						playerTurn = false;
						window.SetTitle((std::string(Constants::APPLICATION_NAME) + Constants::COMPUTER_TURN_SUFFIX).c_str());
					}
				}
				else {
					board = board.Play(computer(board), false);
					playerTurn = true;
					window.SetTitle((std::string(Constants::APPLICATION_NAME) + Constants::PLAYER_TURN_SUFFIX).c_str());
				}

				if (board.BlueWin()) {
					gameOver = true;
					window.SetTitle((std::string(Constants::APPLICATION_NAME) + Constants::PLAYER_WIN_SUFFIX).c_str());
				}
				else if (board.RedWin()) {
					gameOver = true;
					window.SetTitle((std::string(Constants::APPLICATION_NAME) + Constants::COMPUTER_WIN_SUFFIX).c_str());
				}
				else if (board.EmptyBegin() == board.EmptyEnd()) {
					gameOver = true;
					window.SetTitle((std::string(Constants::APPLICATION_NAME) + Constants::DRAW_SUFFIX).c_str());
				}
			}


			// Poll window
			window.Update();

			// Draw board
			renderer.Draw(window, board, playerTurn && !gameOver);
		}
	}
	catch (const std::exception& err) {
		std::cerr << err.what() << '\n';
		GoalFunctionThreadPool::Kill();
		return EXIT_FAILURE;
	}
	GoalFunctionThreadPool::Kill();
	return EXIT_SUCCESS;
}