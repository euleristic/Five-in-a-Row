// This header defines the DecisionComputer class, which runs the minimax algorithm on a thread
// It also has the responsibility of killing the goal function thread pool.

#pragma once

#include "constants.hpp"
#include "goalFunctionThreadPool.hpp"
#include "minimax.hpp"

#include <thread>
#include <optional>

class DecisionComputer {
public:
	DecisionComputer();
	~DecisionComputer();

	// Signals the thread to begin the search, given the board and color of the ply
	void operator()(const Board& board, const bool blue);

	// Returns whether the search is running
	bool Running() const;

	// Returns the result of the search, if available. Does not block
	std::optional<size_t> TryResult() noexcept;

	// Block until the search is complete
	void Await() noexcept;

private:
	bool running{};
	class ThreadPoolKiller {
	public:
		~ThreadPoolKiller() {
			GoalFunctionThreadPool::Kill();
		}
	} killer;
	std::thread thread;
	std::binary_semaphore begin{ 0 };
	std::binary_semaphore complete{ 0 };
	bool dead{};
	
	size_t result;
	const Board* board;
	bool blue;
};