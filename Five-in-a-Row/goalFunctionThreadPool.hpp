#pragma once

#include "constants.hpp"
#include "board.hpp"

#include <semaphore>
#include <thread>

class GoalFunctionThreadPool {
public:
	static GoalFunctionThreadPool& Get();
	static void Kill();
	float operator()(const Board& board);

private:
	static GoalFunctionThreadPool instance;
	GoalFunctionThreadPool();

	std::thread pool[3];
	std::binary_semaphore begin_signals[3] = {
		std::binary_semaphore{0},
		std::binary_semaphore{0},
		std::binary_semaphore{0}
	};
	std::binary_semaphore complete_signals[3] = {
		std::binary_semaphore{0},
		std::binary_semaphore{0},
		std::binary_semaphore{0}
	};
	Board board;
	float results[3];
	bool dead{};
};