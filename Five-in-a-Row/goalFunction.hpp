// This header contains the definition of the goal function
// Essentially, it just wraps the implementation function object
// so that it can have static linkage and be a template argument for testing purposes
#pragma once

#include "board.hpp"
#include "goalFunctionThreadPool.hpp"

static float GoalFunction(const Board& board) {
	return GoalFunctionThreadPool::Get()(&board);
}