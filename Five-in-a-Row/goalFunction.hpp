#pragma once

#include "board.hpp"
#include "goalFunctionThreadPool.hpp"

static float GoalFunction(const Board& board) {
	auto& threadedFunction = GoalFunctionThreadPool::Get();
	return threadedFunction(board);
}