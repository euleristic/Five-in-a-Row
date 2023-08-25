// The minimax implementation is templated, both for unit testing but also for 
// compile-time unrolling of the recursion. For partial template specialization,
// it must be a function object.

#pragma once

#include <utility>
#include <limits>
#include <algorithm>

#include "board.hpp"

// Primary struct declaration

// Minimax search with function F
// if returnChild is true, the best immediate child (position) is returned
// else, the algorithm is agnostic to which of its children is best, simply returning its value
template<size_t depth, bool max, float(*F)(const Board&), bool returnChild = false> struct Minimax;



// General depth, partial specialization returning the tree's value (child agnostic)
template<size_t depth, bool max, float(*F)(const Board&)>
struct Minimax<depth, max, F, false> {
	float operator()(const Board& board,
		float alpha = -std::numeric_limits<float>::infinity(),
		float beta = std::numeric_limits<float>::infinity()) const {

		// If the board is won for either side, we cannot keep looking
		if (auto score = F(board); score == std::numeric_limits<float>::infinity() ||
			score == -std::numeric_limits<float>::infinity()) {
			return score;
		}

		// The minimax of the next depth
		constexpr Minimax<depth - 1, !max, F> next{};

		// Initialize bestScore to worst value, updating as we go
		float bestScore = max ? -std::numeric_limits<float>::infinity() :
			std::numeric_limits<float>::infinity();
		
		// Do we sort the children for this depth?
		// (To sort, we need to call Board::InRangePlies, which may be wasteful at certain depths
		// due to the alpha-beta pruning)
		if constexpr (depth >= Constants::PLY_LOOK_AHEAD - Constants::SORTING_DEPTH) {
			auto order = board.InRangePlies();
			std::sort(order.begin(), order.end(), [&](const size_t lhs, const size_t rhs) -> bool {
				if constexpr (max) {
					return F(board.Play(lhs, !max)) > F(board.Play(rhs, !max));
				}
				else {
					return F(board.Play(lhs, !max)) < F(board.Play(rhs, !max));
				}
			});
			for (size_t ply : order) {
				if (!board.InRange(ply)) continue;
				HandleChildValue(next(board.Play(ply, !max), alpha, beta), bestScore, alpha, beta);
				if constexpr (max) {
					if (bestScore >= beta)
						return bestScore;
				}
				else if (bestScore <= alpha)
					return bestScore;
			}
			return bestScore;
		}
		// No sorting, just iterate through the "in range" children lazily
		for (size_t ply = 0; ply < Constants::BOARD_SIZE; ++ply) {
			if (!board.InRange(ply)) continue;
			HandleChildValue(next(board.Play(ply, !max), alpha, beta), bestScore, alpha, beta);
			if constexpr (max) {
				if (bestScore >= beta)
					return bestScore;
			}
			else if (bestScore <= alpha)
				return bestScore;
		}
		return bestScore;
	}
private:

	// Handles the result of a child minimax search
	void HandleChildValue(const float score, float& bestScore, float& alpha, float& beta) const {
		if constexpr (max) {
			bestScore = std::max(score, bestScore);
			alpha = std::max(alpha, bestScore);
		}
		else {
			bestScore = std::min(score, bestScore);
			beta = std::min(beta, bestScore);
		}
	}
};


// Base case partial specialization
template<bool max, float(*F)(const Board&)>
struct Minimax<0, max, F, false> {
	float operator()(const Board& board, float alpha = 0.0f, float beta = 0.0f) const {
		// Just call the function
		return F(board);
	}
};


// Partial specialization returning best immediate child of tree (value agnostic)
template<size_t depth, bool max, float(*F)(const Board&)>
struct Minimax<depth, max, F, true> {
	size_t operator()(const Board& board,
		float alpha = -std::numeric_limits<float>::infinity(),
		float beta = std::numeric_limits<float>::infinity()) const {

		static_assert(depth != 0); // There is no iterator to return

		// The policy is just to always sort at first depth, because otherwise the result can be strange
		// (e.g. not finishing the game when it can waste turns and still win later)
		auto order = board.InRangePlies();
		std::sort(order.begin(), order.end(), [&](const size_t lhs, const size_t rhs) -> bool {
			if constexpr (max) {
				return F(board.Play(lhs, !max)) > F(board.Play(rhs, !max));
			}
			else {
				return F(board.Play(lhs, !max)) < F(board.Play(rhs, !max));
			}
		});

		constexpr Minimax<depth - 1, !max, F, false> next{}; // Next depth is child-agnostic

		float bestScore = max ? -std::numeric_limits<float>::infinity() :
			std::numeric_limits<float>::infinity();
		auto bestChild = order.front(); // First born favoritism

		// Search the children
		for (const auto ply : order) {
			HandleChildValue(ply, next(board.Play(ply, !max), alpha, beta),
				bestChild, bestScore, alpha, beta);
		}

		// If it's lost to a perfect player no matter what,
		// try to atleast survive the next move (no strange giving up-behavior)
		if constexpr (depth != 2) {
			if ((max && bestScore == -std::numeric_limits<float>::infinity()) ||
				(!max && bestScore == std::numeric_limits<float>::infinity())) {
				constexpr Minimax<2, !max, F, true> desperateTry{};
				return desperateTry(board);
			}
		}

		return bestChild;
	}
private:
	// Handle minimax search of child, retaining the knowledge of which child is best
	void HandleChildValue(const size_t child, const float score,
		size_t& bestChild, float& bestScore, float& alpha, float& beta) const {
		if constexpr (max) {
			if (score > bestScore) {
				bestScore = score;
				bestChild = child;
			}
			alpha = std::max(alpha, bestScore);
		}
		else {
			if (score < bestScore) {
				bestScore = score;
				bestChild = child;
			}
			beta = std::min(beta, bestScore);
		}
	}
};