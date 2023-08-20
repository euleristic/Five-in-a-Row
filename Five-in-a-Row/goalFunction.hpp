#pragma once

#include "board.hpp"
#include "goalFunctionThreadPool.hpp"

static float GoalFunction(const Board& board) {
	return GoalFunctionThreadPool::Get()(board);
}