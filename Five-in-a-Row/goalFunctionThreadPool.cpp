#include "goalFunctionThreadPool.hpp"

#include <iostream>

using namespace Constants;

GoalFunctionThreadPool GoalFunctionThreadPool::instance;

GoalFunctionThreadPool& GoalFunctionThreadPool::Get() {
	return instance;
}

float GoalFunctionSubRange(const Board::FivesIterator& begin, const Board::FivesIterator& end) {
	float sum{};
	for (auto it = begin; it != end; ++it) {
		if (const auto score = *it /*;score != 0*/) {
			if (score == 5)
				return Constants::SCORE_MAP[4];
			if (score == -5)
				return -Constants::SCORE_MAP[4];
			if (score > 0) {
				sum += Constants::SCORE_MAP[score - 1];
			}
			else {
				sum -= Constants::SCORE_MAP[-score - 1];
			}
		}
	}
	return sum;
}

GoalFunctionThreadPool::GoalFunctionThreadPool() {
	pool[0] = std::thread([&]() {
		while (true) {
			begin_signals[0].acquire();
			if (dead) {
				return;
			}
			results[0] = GoalFunctionSubRange(board.FivesBegin<0>(), board.FivesBegin<1>());
			complete_signals[0].release();
		}
	});
	pool[1] = std::thread([&]() {
		while (true) {
			begin_signals[1].acquire();
			if (dead)
				return;
			results[1] = GoalFunctionSubRange(board.FivesBegin<1>(), board.FivesBegin<2>());
			complete_signals[1].release();
		}
	});
	pool[2] = std::thread([&]() {
		while (true) {
			begin_signals[2].acquire();
			if (dead)
				return;
			results[2] = GoalFunctionSubRange(board.FivesBegin<2>(), board.FivesBegin<3>());
			complete_signals[2].release();
		}
	});
}

void GoalFunctionThreadPool::Kill() {
	instance.dead = true;
	for (size_t i = 0; i < 3; ++i) {
		instance.begin_signals[i].release();
		instance.pool[i].join();
	}
}

float GoalFunctionThreadPool::operator()(const Board& board) {
	this->board = board;

	for (size_t i = 0; i < 3; ++i) {
		begin_signals[i].release();
	}
	float mainResult = GoalFunctionSubRange(board.FivesBegin<3>(), board.FivesEnd());
	for (size_t i = 0; i < 3; ++i) {
		complete_signals[i].acquire();
	}
	constexpr float inf = std::numeric_limits<float>::infinity();
	if (mainResult == inf || results[0] == inf || results[1] == inf || results[2] == inf) {
		return inf;
	}
	if (mainResult == -inf || results[0] == -inf || results[1] == -inf || results[2] == -inf) {
		return -inf;
	}
	return mainResult + results[0] + results[1] + results[2];
}