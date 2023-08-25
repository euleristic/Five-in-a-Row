// This header defines the goal function thread pool function object, which implements the goal function
// and manages a thread pool. It is a singleton, so that it can be used statically.

// Global resource management causes spaghetti code. The singleton cannot kill it's own threads,
// because it's destructor won't be called until it's threads stop running. I wanted the static
// linkage for testing and threaded implementation for speed, which came at this cost.
// The DecisionComputer class kills the threadpool in it's destructor.

#pragma once

#include "constants.hpp"
#include "board.hpp"

#include <semaphore>
#include <thread>

class GoalFunctionThreadPool {
public:
	// Returns a reference to the instance
	static GoalFunctionThreadPool& Get();

	// Kills the thread pool
	static void Kill();

	// Returns the value of the goal function for the board
	float operator()(const Board* board);

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
	const Board* board;
	float results[3];
	bool dead{};
};