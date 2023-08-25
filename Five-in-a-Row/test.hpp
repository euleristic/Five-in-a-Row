#pragma once

#include "constants.hpp"
#include "board.hpp"

#include <semaphore>
#include <thread>

class GoalFunctionThreadPoolTest {
public:
	static GoalFunctionThreadPoolTest& Get();
	static void Kill();
	float operator()(const Board* board);

private:
	static GoalFunctionThreadPoolTest instance;
	GoalFunctionThreadPoolTest();

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

// constexpr int class_size = sizeof(GoalFunctionThreadPool);
// constexpr int pool_size = sizeof(std::thread[3]);
// constexpr int pointer_size = sizeof(const Board*);
// constexpr int semaphores_size = sizeof(std::binary_semaphore[3]);
// constexpr int results_size = sizeof(float[3]);
// constexpr int bool_size = sizeof(bool);
