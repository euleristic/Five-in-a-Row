#include "test.hpp"

#include <numeric>
#include <fstream>

using namespace Constants;

//GoalFunctionThreadPoolTest GoalFunctionThreadPoolTest::instance;

GoalFunctionThreadPoolTest& GoalFunctionThreadPoolTest::Get() {
	return instance;
}

constexpr float TEST_MAP[] = {
	1.0f * 1.0f * 1.0f * 1.0f,
	2.0f * 2.0f * 2.0f * 2.0f,
	3.0f * 3.0f * 3.0f * 3.0f,
	4.0f * 4.0f * 4.0f * 4.0f,
	std::numeric_limits<float>::infinity()
};

template <typename FivesIterator>
float GoalFunctionSubRangeTest(const FivesIterator& begin, const FivesIterator& end) {
	return std::reduce(begin, end, 0.0f, [](const float sum, const int8_t score) -> float {
		if (score) {
			if (score > 0) {
				return sum + TEST_MAP[score - 1];
			}
			else {
				return sum - TEST_MAP[-score - 1];
			}
		}
		else {
			return sum;
		}
	});
	float sum{};
	for (auto it = begin; it != end; ++it) {
		if (const auto score = *it /*;score != 0*/) {
			if (score > 0) {
				sum += TEST_MAP[score - 1];
			}
			else {
				sum -= TEST_MAP[-score - 1];
			}
		}
	}
	return sum;
}

GoalFunctionThreadPoolTest::GoalFunctionThreadPoolTest() {
	pool[0] = std::thread([&]() {
		while (true) {
			begin_signals[0].acquire();
			if (dead) {
				return;
			}
			results[0] = GoalFunctionSubRangeTest(board->HorizontalFivesBegin(), board->HorizontalFivesEnd());
			complete_signals[0].release();
		}
	});
	pool[1] = std::thread([&]() {
		while (true) {
			begin_signals[1].acquire();
			if (dead)
				return;
			results[1] = GoalFunctionSubRangeTest(board->VerticalFivesBegin(), board->VerticalFivesEnd());
			complete_signals[1].release();
		}
	});
	pool[2] = std::thread([&]() {
		while (true) {
			begin_signals[2].acquire();
			if (dead)
				return;
			results[2] = GoalFunctionSubRangeTest(board->SouthwestFivesBegin(), board->SouthwestFivesEnd());
			complete_signals[2].release();
		}
	});
}

void GoalFunctionThreadPoolTest::Kill() {
	instance.dead = true;
	for (size_t i = 0; i < 3; ++i) {
		instance.begin_signals[i].release();
		instance.pool[i].join();
	}
}

float GoalFunctionThreadPoolTest::operator()(const Board* board) {
	this->board = board;
	for (size_t i = 0; i < 3; ++i) {
		begin_signals[i].release();
	}
	float mainResult = GoalFunctionSubRangeTest(board->SoutheastFivesBegin(), board->SoutheastFivesEnd());
	for (size_t i = 0; i < 3; ++i) {
		complete_signals[i].acquire();
	}
	return mainResult + results[0] + results[1] + results[2];
}