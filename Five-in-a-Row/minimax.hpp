// The minimax implementation is templated, both for unit testing but also for 
// compile-time unrolling of the recursion. For partial template specialization,
// it must be a function object.

#pragma once

#include <concepts>
#include <limits>

#include "board.hpp"

// Primary struct declarations

template<size_t depth, bool max, float(*F)(const Board&), 
	bool prune = false, bool returnChild = false> struct Minimax;

// General case, no pruning

template<size_t depth, bool max, float(*F)(const Board&)>
struct Minimax<depth, max, F, false, false> {
	float operator()(const Board& board) const {
		constexpr Minimax<depth - 1, !max, F> next{};
		if constexpr (max) {
			float maxScore = -std::numeric_limits<float>::infinity();
			for (auto it = board.InRangeBegin(); it != board.InRangeEnd(); ++it) {
				maxScore = std::max(maxScore, next(board.Play(it, !max)));
			}
			return maxScore;
		}
		else {
			float minScore = std::numeric_limits<float>::infinity();
			for (auto it = board.InRangeBegin(); it != board.InRangeEnd(); ++it) {
				minScore = std::min(minScore, next(board.Play(it, !max)));
			}
			return minScore;
		}
	}
};

// Base case, no prune

template<bool max, float(*F)(const Board&)>
struct Minimax<0, max, F, false, false> {
	float operator()(const Board& board) const {
		return F(board);
	}
};

// General case, with alpha-beta pruning

template<size_t depth, bool max, float(*F)(const Board&)>
struct Minimax<depth, max, F, true, false> {
	float operator()(const Board& board, 
		float alpha = -std::numeric_limits<float>::infinity(), 
		float beta = std::numeric_limits<float>::infinity()) const {

		// If a board is won for either side, we can't keep looking
		if (auto score = F(board); score == std::numeric_limits<float>::infinity() ||
			score == -std::numeric_limits<float>::infinity()) {
			return score;
		}

		constexpr Minimax<depth - 1, !max, F, true> next{};
		if constexpr (max) {
			float maxScore = -std::numeric_limits<float>::infinity();
			for (auto it = board.InRangeBegin(); it != board.InRangeEnd(); ++it) {
				maxScore = std::max(maxScore, next(board.Play(it, !max), alpha, beta));
				if (maxScore >= beta) {
					return maxScore;
				}
				alpha = std::max(alpha, maxScore);
			}
			return maxScore;
		}
		else {
			float minScore = std::numeric_limits<float>::infinity();
			for (auto it = board.InRangeBegin(); it != board.InRangeEnd(); ++it) {
				minScore = std::min(minScore, next(board.Play(it, !max), alpha, beta));
				if (minScore <= alpha) {
					return minScore;
				}
				beta = std::min(beta, minScore);
			}
			return minScore;
		}
	}
};

// Base case, with prune and dummy alpha/beta defaults, since they do nothing

template<bool max, float(*F)(const Board&)>
struct Minimax<0, max, F, true, false> {
	float operator()(const Board& board, float alhpa = 0.0f, float beta = 0.0f) const {
		return F(board);
	}
};

// Return child, alpha-beta

template<size_t depth, bool max, float(*F)(const Board&)>
struct Minimax<depth, max, F, true, true> {
	Board::InRangeIterator operator()(const Board& board,
		float alpha = -std::numeric_limits<float>::infinity(),
		float beta = std::numeric_limits<float>::infinity()) const {

		static_assert(depth != 0); // There is no iterator to return
		
		constexpr float factor = max ? 1.0f : -1.0f;

		std::pair<Board::InRangeIterator, float> optimalChild = std::make_pair(board.InRangeBegin(), 0.0f);
		std::pair<Board::InRangeIterator, float> secondOptimalChild = std::make_pair(board.InRangeBegin(), 0.0f);

		auto it = board.InRangeBegin();
		float first = F(board.Play(it, !max)) * factor;
		++it;
		if (it == board.InRangeEnd()) {
			return board.InRangeBegin();
		}
		float second = F(board.Play(it, !max)) * factor;

		if (first > second) {
			optimalChild = std::make_pair(board.InRangeBegin(), first);
			secondOptimalChild = std::make_pair(it, second);
		}
		else {
			optimalChild = std::make_pair(it, second);
			secondOptimalChild = std::make_pair(board.InRangeBegin(), first);
		}

		for (++it; it != board.InRangeEnd(); ++it) {
			float score = F(board.Play(it, !max)) * factor;
			if (score > optimalChild.second) {
				secondOptimalChild = optimalChild;
				optimalChild = std::make_pair(it, score);
			}
			else if (score > secondOptimalChild.second) {
				secondOptimalChild = std::make_pair(it, score);
			}
		}

		if (optimalChild.second == std::numeric_limits<float>::infinity() * factor) {
			return optimalChild.first;
		}

		constexpr Minimax<depth - 1, !max, F, true, false> next{}; // Next depth returns float


		if constexpr (max) {
			float maxScore = -std::numeric_limits<float>::infinity();
			auto bestChild = board.InRangeBegin(); // First born favoritism

			float score = next(board.Play(optimalChild.first, !max), alpha, beta);
			if (score > maxScore) {
				maxScore = score;
				bestChild = optimalChild.first;
			}
			alpha = std::max(alpha, maxScore);

			score = next(board.Play(secondOptimalChild.first, !max), alpha, beta);
			if (score > maxScore) {
				maxScore = score;
				bestChild = secondOptimalChild.first;
			}
			alpha = std::max(alpha, maxScore);

			for (it = board.InRangeBegin(); it != board.InRangeEnd(); ++it) {
				if (it == optimalChild.first || it == secondOptimalChild.first) continue;
				score = next(board.Play(it, !max), alpha, beta);
				if (score > maxScore) {
					maxScore = score;
					bestChild = it;
				}
				alpha = std::max(alpha, maxScore);
			}
			return bestChild;
		}
		else {
			float minScore = std::numeric_limits<float>::infinity();
			auto bestChild = board.InRangeBegin();

			float score = next(board.Play(optimalChild.first, !max), alpha, beta);
			if (score < minScore) {
				minScore = score;
				bestChild = optimalChild.first;
			}
			beta = std::min(beta, minScore);

			score = next(board.Play(secondOptimalChild.first, !max), alpha, beta);
			if (score < minScore) {
				minScore = score;
				bestChild = secondOptimalChild.first;
			}
			beta = std::min(beta, minScore);

			for (it = board.InRangeBegin(); it != board.InRangeEnd(); ++it) {
				if (it == optimalChild.first || it == secondOptimalChild.first) continue;
				float score = next(board.Play(it, !max), alpha, beta);
				if (score < minScore) {
					minScore = score;
					bestChild = it;
				}
				beta = std::min(beta, minScore);
			}
			return bestChild;
		}
	}
};

// Return child, no alpha-beta

template<size_t depth, bool max, float(*F)(const Board&)>
struct Minimax<depth, max, F, false, true> {
	Board::InRangeIterator operator()(const Board& board) const {

		static_assert(depth != 0); // There is no iterator to return
		constexpr Minimax<depth - 1, !max, F, false, false> next{}; // Next depth returns float

		if constexpr (max) {
			float maxScore = -std::numeric_limits<float>::infinity();
			auto bestChild = board.InRangeBegin();
			for (auto it = board.InRangeBegin(); it != board.InRangeEnd(); ++it) {
				float score = next(board.Play(it, !max));
				if (score > maxScore) {
					maxScore = score;
					bestChild = it;
				}
			}
			return bestChild;
		}
		else {
			float minScore = std::numeric_limits<float>::infinity();
			auto bestChild = board.InRangeBegin();
			for (auto it = board.InRangeBegin(); it != board.InRangeEnd(); ++it) {
				float score = next(board.Play(it, !max));
				if (score < minScore) {
					minScore = score;
					bestChild = it;
				}
			}
			return bestChild;
		}
	}
};
