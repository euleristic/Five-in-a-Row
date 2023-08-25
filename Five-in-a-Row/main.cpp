// Requires C++20 and is dependent on GLFW, GlAD and glm
// I would use modules, but unfortunately Visual Studio does not yet play nice with
// a mix of #includes and imports. Soon!

// How to play:
// Try to get five of your pieces (blue) in a sequence before I do (red).
// They may be consecutive horizontally, vertically or diagonally.
// To place a piece, simply left-click the cell you wish to place it upon (it will draw a ghost piece).
// If you make a mistake you can press the Z-key to undo a move.
// If you don't know what to do, press the H-key and I'll help you.
// If you wish to start the game again, press the R-key (we'll take turns starting)
// Good luck! You'll need it

// If you get bored, or it's too hard to beat me or I'm too slow, go mess with the settings in "constants.hpp"

#include "scopedLibrary.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include "decisionComputer.hpp"

#include <fstream>
#include <stack>

auto main() -> int {
	try {
		DecisionComputer computer;

		ScopedLibrary lib;
		Window window;
		Renderer renderer;

		Board board;
		std::stack<size_t> plies;

		bool playerFirst = Constants::PLAYER_FIRST;
		bool playerTurn = playerFirst;
		bool gameOver = false;

		// Main loop
		while (!window.ShouldClose()) {
			// Game logic
			if (!gameOver) {

				// Are we awaiting a decision from the computer?
				if (computer.Running()) {

					// Has it reached the decision?
					if (auto ply = computer.TryResult()) {
						plies.push(*ply);
						board = board.Play(*ply, playerTurn);
						playerTurn = !playerTurn;
						window.SetTitle((std::string(Constants::APPLICATION_NAME) + (playerTurn ?
							Constants::PLAYER_TURN_SUFFIX : Constants::COMPUTER_TURN_SUFFIX)).c_str());
					}
				}
				else {
					if (playerTurn) {

						// Has the player clicked an empty cell?
						if (const auto selected = board.Selected(window.CursorPosition()); 
							selected && window.Clicked()) {
							plies.push(*selected);
							board = board.Play(*selected, true);
							playerTurn = false;
							window.SetTitle((std::string(Constants::APPLICATION_NAME) + Constants::COMPUTER_TURN_SUFFIX).c_str());
						}

						// Has the player asked for help?
						else if (window.HelpPressed()) {
							window.SetTitle((std::string(Constants::APPLICATION_NAME) + Constants::COMPUTER_TURN_SUFFIX).c_str());
							computer(board, true);
						}

						// Has the player requested to reset the board?
						if (window.ResetPressed()) {
							// No threads left behind
							if (computer.Running()) {
								computer.Await();
							}
							board = Board{};
							gameOver = false;
							playerFirst = !playerFirst;
							playerTurn = playerFirst;
							plies = {};
							window.SetTitle((std::string(Constants::APPLICATION_NAME) + (playerTurn ?
								Constants::PLAYER_TURN_SUFFIX : Constants::COMPUTER_TURN_SUFFIX)).c_str());
						}

						// Has the player requested to undo the top move?
						if (plies.size() > 1 && window.UndoPressed()) {
							board = board.Reset(plies.top());
							plies.pop();
							board = board.Reset(plies.top());
							plies.pop();
							gameOver = false;
							window.SetTitle((std::string(Constants::APPLICATION_NAME) + (playerTurn ?
								Constants::PLAYER_TURN_SUFFIX : Constants::COMPUTER_TURN_SUFFIX)).c_str());
						}
					}
					else {
						// It's the computer's turn, notify it to begin computation
						computer(board, false);
					}
				}

				// Check that the game is not over
				if (board.BlueWin()) {
					gameOver = true;
					window.SetTitle((std::string(Constants::APPLICATION_NAME) + Constants::PLAYER_WIN_SUFFIX).c_str());
				}
				else if (board.RedWin()) {
					gameOver = true;
					window.SetTitle((std::string(Constants::APPLICATION_NAME) + Constants::COMPUTER_WIN_SUFFIX).c_str());
				}
				else if (board.Full()) {
					gameOver = true;
					window.SetTitle((std::string(Constants::APPLICATION_NAME) + Constants::DRAW_SUFFIX).c_str());
				}
			}

			// Poll window and update input state
			window.Update();

			// Draw board
			renderer.Draw(window, board, playerTurn && !gameOver, !plies.empty() ?
				std::make_optional(plies.top()) : std::nullopt);
		}
	}
	catch (const std::exception& err) {
		// Write the error to a file
		std::ofstream errorFile("errorFile.txt");
		errorFile << err.what();
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}