#include "goalFunctionThreadPool.hpp"

#include <numeric>
#include <execution>

using namespace Constants;

// This is the fastest policy on my machine
constexpr auto policy = std::execution::unseq;

GoalFunctionThreadPool GoalFunctionThreadPool::instance;

GoalFunctionThreadPool& GoalFunctionThreadPool::Get() {
	return instance;
}

// Counts the progress of all "fives" along an orientation, then transforms them according
// to SCORE_MAP, before summing them
template <FivesOrientation orientation> float GoalFunctionSubSet(const Board& board);

template<>
float GoalFunctionSubSet<FivesOrientation::HORIZONTAL>(const Board& board) {
	return std::transform_reduce(policy,
		HORIZONTAL_FIVES_ROOTS.begin(), HORIZONTAL_FIVES_ROOTS.end(),
		0.0f, std::plus<float>(), [&](const size_t index) -> float {
		auto score = board.CountFive<FivesOrientation::HORIZONTAL>(index);
		if (score) {
			if (score > 0) return Constants::SCORE_MAP[score - 1];
			else return -Constants::SCORE_MAP[-score - 1];
		}
		else return 0.0f;
	});
}

template<>
float GoalFunctionSubSet<FivesOrientation::VERTICAL>(const Board& board) {
	return std::transform_reduce(policy,
		VERTICAL_FIVES_ROOTS.begin(), VERTICAL_FIVES_ROOTS.end(),
		0.0f, std::plus<float>(), [&](const size_t index) -> float {
		auto score = board.CountFive<FivesOrientation::VERTICAL>(index);
		if (score) {
			if (score > 0) return Constants::SCORE_MAP[score - 1];
			else return -Constants::SCORE_MAP[-score - 1];
		}
		else return 0.0f;
	});
}


template<>
float GoalFunctionSubSet<FivesOrientation::SOUTHEAST>(const Board& board) {
	return std::transform_reduce(policy,
		SOUTHEAST_FIVES_ROOTS.begin(), SOUTHEAST_FIVES_ROOTS.end(),
		0.0f, std::plus<float>(), [&](const size_t index) -> float {
		auto score = board.CountFive<FivesOrientation::SOUTHEAST>(index);
		if (score) {
			if (score > 0) return Constants::SCORE_MAP[score - 1];
			else return -Constants::SCORE_MAP[-score - 1];
		}
		else return 0.0f;
	});
}


template<>
float GoalFunctionSubSet<FivesOrientation::SOUTHWEST>(const Board& board) {
	return std::transform_reduce(policy, 
		SOUTHWEST_FIVES_ROOTS.begin(), SOUTHWEST_FIVES_ROOTS.end(),
		0.0f, std::plus<float>(), [&](const size_t index) -> float {
		auto score = board.CountFive<FivesOrientation::SOUTHWEST>(index);
		if (score) {
			if (score > 0) return Constants::SCORE_MAP[score - 1];
			else return -Constants::SCORE_MAP[-score - 1];
		}
		else return 0.0f;
	});
}

GoalFunctionThreadPool::GoalFunctionThreadPool() {

	// Run the threads

	pool[0] = std::thread([&]() {
		while (true) {
			begin_signals[0].acquire();
			if (dead) {
				return;
			}
			results[0] = GoalFunctionSubSet<FivesOrientation::HORIZONTAL>(*board);
			complete_signals[0].release();
		}
	});
	pool[1] = std::thread([&]() {
		while (true) {
			begin_signals[1].acquire();
			if (dead)
				return;
			results[1] = GoalFunctionSubSet<FivesOrientation::VERTICAL>(*board);
			complete_signals[1].release();
		}
	});
	pool[2] = std::thread([&]() {
		while (true) {
			begin_signals[2].acquire();
			if (dead)
				return;
			results[2] = GoalFunctionSubSet<FivesOrientation::SOUTHWEST>(*board);
			complete_signals[2].release();
		}
	});
}

void GoalFunctionThreadPool::Kill() {
	instance.dead = true;
	// Await the threads
	for (size_t i = 0; i < 3; ++i) {
		instance.begin_signals[i].release();
		instance.pool[i].join();
	}
}

float GoalFunctionThreadPool::operator()(const Board* board) {
	this->board = board;
	for (size_t i = 0; i < 3; ++i) {
		begin_signals[i].release();
	}
	// The fourth orientation is run on this thread
	float mainResult = GoalFunctionSubSet<FivesOrientation::SOUTHEAST>(*board);
	for (size_t i = 0; i < 3; ++i) {
		complete_signals[i].acquire();
	}
	return mainResult + results[0] + results[1] + results[2];
}