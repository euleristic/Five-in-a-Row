#include "CppUnitTest.h"

#include "../Five-in-a-Row/board.hpp"
#include "../Five-in-a-Row/minimax.hpp"
#include "../Five-in-a-Row/goalFunction.hpp"

#include <format>
#include <vector>

namespace Microsoft::VisualStudio::CppUnitTestFramework {


	template <>
	static std::wstring ToString<CellState>(const CellState& state) {
		switch (state) {
		case CellState::BLUE: return L"CellState::BLUE";
		case CellState::RED: return L"CellState::RED";
		default: return L"CellState::EMPTY";
		}
	}
	
	template <>
	static std::wstring ToString<std::pair<size_t, size_t>>(const std::pair<size_t, size_t>& pair) {
		return std::format(L"({}, {})", pair.first, pair.second);
	}
}