//#pragma comment(linker, "/SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup")

#include "scopedLibrary.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include "board.hpp"
#include "minimax.hpp"
#include "goalFunction.hpp"

#include <fstream>
#include <vector>
#include <stack>

auto main() -> int {

	try {
		ScopedLibrary lib;
		Window window;
		Renderer renderer;

		Board board;
		std::stack<std::pair<size_t, size_t>> moves;

		constexpr Minimax<Constants::PLY_LOOK_AHEAD, true, GoalFunction, true> redComputer;
		constexpr Minimax<Constants::PLY_LOOK_AHEAD, false, GoalFunction, true> blueComputer;

		bool playerFirst = Constants::PLAYER_FIRST;
		bool playerTurn = playerFirst;
		bool gameOver = false;

		while (!window.ShouldClose()) {

			// Game logic
			if (!gameOver) {
				if (playerTurn) {
					if (const auto selected = board.Selected(window.CursorPosition()); selected != board.EmptyEnd() && window.Clicked()) {
						moves.push(selected.Position());
						board = board.Play(selected, true);
						playerTurn = false;
						window.SetTitle((std::string(Constants::APPLICATION_NAME) + Constants::COMPUTER_TURN_SUFFIX).c_str());
					}
					else if (window.HelpPressed()) {
						window.SetTitle((std::string(Constants::APPLICATION_NAME) + Constants::COMPUTER_TURN_SUFFIX).c_str());
						auto move = blueComputer(board);
						moves.push(move.Position());
						board = board.Play(move, true);
						playerTurn = false;
					}
				}
				else {
					auto move = redComputer(board);
					moves.push(move.Position());
					board = board.Play(move, false);
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

			if (window.ResetPressed()) {
				board = Board{};
				gameOver = false;
				playerFirst = !playerFirst;
				playerTurn = playerFirst;
				moves = {};
				window.SetTitle((std::string(Constants::APPLICATION_NAME) + (playerTurn ?
					Constants::PLAYER_TURN_SUFFIX : Constants::COMPUTER_TURN_SUFFIX)).c_str());
			}

			if (moves.size() > 1 && window.UndoPressed()) {
				board = board.Reset(moves.top());
				moves.pop();
				board = board.Reset(moves.top());
				moves.pop();
				gameOver = false;
				window.SetTitle((std::string(Constants::APPLICATION_NAME) + (playerTurn ?
					Constants::PLAYER_TURN_SUFFIX : Constants::COMPUTER_TURN_SUFFIX)).c_str());
			}

			// Poll window
			window.Update();

			// Draw board
			renderer.Draw(window, board, playerTurn && !gameOver, !moves.empty() ?
				std::make_optional(moves.top()) : std::nullopt);
		}
	}
	catch (const std::exception& err) {
		std::ofstream errorFile("errorFile.txt");
		errorFile << err.what();
		GoalFunctionThreadPool::Kill();
		return EXIT_FAILURE;
	}
	GoalFunctionThreadPool::Kill();
	return EXIT_SUCCESS;
}