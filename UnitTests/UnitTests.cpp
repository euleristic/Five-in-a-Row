#include "pch.h"
#include "UnitTests.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace Constants;

// Returns 1.0f if red won, -1.0f if blue, and 0.0f if neither has won.
float SimpleGoal(const Board&);
constexpr float EPSILON = 0.1f;

namespace UnitTests {

	TEST_CLASS(UnitTests)
	{
	public:

		// The tests assume a 15x15 board and may fail otherwise
		TEST_METHOD(BadBoardConstructorThrows) {
			// Size
			Assert::ExpectException<std::runtime_error>([]() {
				Board b("*");
			});
			Assert::ExpectException<std::runtime_error>([]() {
				Board b("*********");
			});
			Assert::ExpectException<std::runtime_error>([]() {
				Board b(std::string() +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"****************");
			});

			// Illegal characters
			Assert::ExpectException<std::runtime_error>([]() {
				Board b(std::string() +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"*******b*******" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************");
			});

			Assert::ExpectException<std::runtime_error>([]() {
				Board b(std::string() +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"*******f*******" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************");
			});
		}

		TEST_METHOD(BoardAt) {
			// Just check whether at behaves correctly for all positions of some arbitrary board
			Board b(std::string() +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***********B***" +
				"*******R*******" +
				"*B*************" +
				"***************" +
				"***********R***" +
				"***************" +
				"***************" +
				"***************" +
				"***************");

			for (size_t j = 0; j < BOARD_HEIGHT; ++j) {
				for (size_t i = 0; i < BOARD_WIDTH; ++i) {
					CellState expected = CellState::EMPTY;
					if (i == 11 && j == 6) expected = CellState::BLUE;
					else if (i == 7 && j == 7) expected = CellState::RED;
					else if (i == 1 && j == 8) expected = CellState::BLUE;
					else if (i == 11 && j == 10) expected = CellState::RED;
					Assert::AreEqual(expected, b.At(i, j), std::format(L"x = {}, y = {}", i, j).c_str());
					Assert::AreEqual(expected, b.At(std::make_pair(i, j)), std::format(L"x = {}, y = {}", i, j).c_str());
				}
			}

			//Assert::ExpectException<std::runtime_error>([&b]() { b.At(15, 4); });
			//Assert::ExpectException<std::runtime_error>([&b]() { b.At(10, 15); });
		}

		TEST_METHOD(BoardAllIterator) {
			Board b(std::string() +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***********B***" +
				"*******R*******" +
				"*B*************" +
				"***************" +
				"***********R***" +
				"***************" +
				"***************" +
				"***************" +
				"***************");

			for (auto it = b.AllBegin(); it != b.AllEnd(); ++it) {
				const auto [x, y] = it.Position();
				CellState expected = CellState::EMPTY;
				if (x == 11 && y == 6) Assert::AreEqual(CellState::BLUE, *it, std::format(L"x = {}, y = {}", x, y).c_str());
				else if (x == 7 && y == 7) Assert::AreEqual(CellState::RED, *it, std::format(L"x = {}, y = {}", x, y).c_str());
				else if (x == 1 && y == 8) Assert::AreEqual(CellState::BLUE, *it, std::format(L"x = {}, y = {}", x, y).c_str());
				else if (x == 11 && y == 10) Assert::AreEqual(CellState::RED, *it, std::format(L"x = {}, y = {}", x, y).c_str());
				else Assert::AreEqual(CellState::EMPTY, *it, std::format(L"x = {}, y = {}", x, y).c_str());
			}
		}

		TEST_METHOD(BoardEmptyIterator) {
			Board b(std::string() +
				"B**************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***********B***" +
				"*******R*******" +
				"*B*************" +
				"***************" +
				"***********R***" +
				"***************" +
				"***************" +
				"***************" +
				"***************");

			const auto [x, y] = b.EmptyBegin().Position();
			Assert::AreEqual(static_cast<size_t>(1), x);
			Assert::AreEqual(static_cast<size_t>(0), y);

			size_t count{};
			for (auto it = b.EmptyBegin(); it != b.EmptyEnd(); ++it) {
				Assert::AreEqual(CellState::EMPTY, *it, std::format(L"x = {}, y = {}", it.Position().first, it.Position().second).c_str());
				++count;
			}
			Assert::AreEqual(count, static_cast<size_t>(220));

			Board full(std::string() +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB");

			Assert::AreEqual(full.EmptyBegin(), full.EmptyEnd());

			Assert::AreEqual(static_cast<size_t>(0), Board().EmptyBegin().Position().first);
		}

		TEST_METHOD(BoardInRangeIterator) {
			if constexpr (RANGE == 4) {

				Board b(std::string() +
					"B**************" +			//B    **********		    4
					"***************" +			//     **********			5		  
					"***************" +			//     **        			5 + 8
					"***************" +			//     *         			5 + 9
					"***************" +			//     *         			5 + 9			 
					"***************" +			//******         			9			 
					"***********B***" +			//******     B   			8
					"**********R****" +			//******    R    			8
					"***************" +			//******         			9
					"***************" +			//               			15
					"***************" +			//               			15
					"***************" +			//               			15
					"***************" +			//        *******			8
					"***R***********" +			//   R    *******			7
					"***************");			//        *******			8

				size_t count{};

				size_t counts[15]{};

				for (auto it = b.InRangeBegin(); it != b.InRangeEnd(); ++it) {
					++count;
					Assert::AreEqual(CellState::EMPTY, *it);
					const auto [x, y] = it.Position();
					++counts[y];

					switch (y) {
					case 0:
						Assert::IsTrue(x != 0 && x <= 4, std::format(L"x = {}, y = {}", x, y).c_str());
						break;
					case 1:
						Assert::IsTrue(x <= 4, std::format(L"x = {}, y = {}", x, y).c_str());
						break;
					case 2:
						Assert::IsTrue(x <= 4 || 7 <= x, std::format(L"x = {}, y = {}", x, y).c_str());
						break;
					case 3: case 4:
						Assert::IsTrue(x <= 4 || 6 <= x, std::format(L"x = {}, y = {}", x, y).c_str());
						break;
					case 5:
						Assert::IsTrue(6 <= x, std::format(L"x = {}, y = {}", x, y).c_str());
						break;
					case 6:
						Assert::IsTrue(6 <= x && x != 11, std::format(L"x = {}, y = {}", x, y).c_str());
						break;
					case 7:
						Assert::IsTrue(6 <= x && x != 10, std::format(L"x = {}, y = {}", x, y).c_str());
						break;
					case 8:
						Assert::IsTrue(6 <= x, std::format(L"x = {}, y = {}", x, y).c_str());
						break;
					case 12:
						Assert::IsTrue(x <= 8, std::format(L"x = {}, y = {}", x, y).c_str());
						break;
					case 13:
						Assert::IsTrue(x != 3 && x <= 8, std::format(L"x = {}, y = {}", x, y).c_str());
						break;
					case 14:
						Assert::IsTrue(x <= 8, std::format(L"x = {}, y = {}", x, y).c_str());
						break;
					}
				}
				Assert::AreEqual(static_cast<size_t>(4), counts[0], L"y = 0");
				Assert::AreEqual(static_cast<size_t>(5), counts[1], L"y = 1");
				Assert::AreEqual(static_cast<size_t>(13), counts[2], L"y = 2");
				Assert::AreEqual(static_cast<size_t>(14), counts[3], L"y = 3");
				Assert::AreEqual(static_cast<size_t>(14), counts[4], L"y = 4");
				Assert::AreEqual(static_cast<size_t>(9), counts[5], L"y = 5");
				Assert::AreEqual(static_cast<size_t>(8), counts[6], L"y = 6");
				Assert::AreEqual(static_cast<size_t>(8), counts[7], L"y = 7");
				Assert::AreEqual(static_cast<size_t>(9), counts[8], L"y = 8");
				Assert::AreEqual(static_cast<size_t>(15), counts[9], L"y = 9");
				Assert::AreEqual(static_cast<size_t>(15), counts[10], L"y = 10");
				Assert::AreEqual(static_cast<size_t>(15), counts[11], L"y = 11");
				Assert::AreEqual(static_cast<size_t>(8), counts[12], L"y = 12");
				Assert::AreEqual(static_cast<size_t>(7), counts[13], L"y = 13");
				Assert::AreEqual(static_cast<size_t>(8), counts[14], L"y = 14");
			}

			Board full(std::string() +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB" +
				"BBBBBBBBBBBBBBB");

			Assert::AreEqual(full.InRangeBegin(), full.InRangeEnd());

			Assert::AreEqual(static_cast<size_t>(0), Board().InRangeBegin().Position().first);
		}

		TEST_METHOD(BoardPlay) {
			Board b0(std::string() +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***********B***" +
				"*******R*******" +
				"*B*************" +
				"***************" +
				"***********R***" +
				"***************" +
				"***************" +
				"***************" +
				"***************");

			auto b1 = b0.Play(5, 3, true);
			auto b2 = b1.Play(3, 5, false);

			Assert::AreEqual(CellState::BLUE, b2.At(5, 3));
			Assert::AreEqual(CellState::RED, b2.At(3, 5));
			Assert::AreEqual(CellState::EMPTY, b0.At(5, 3));
			Assert::AreEqual(CellState::EMPTY, b1.At(3, 5));

			size_t pos{};

			for (auto it = b2.EmptyBegin(); it != b2.EmptyEnd(); ++it, ++pos) {
				Board b3 = b2.Play(it, pos % 2);
				Assert::AreEqual((pos % 2) ? CellState::BLUE : CellState::RED, b3.At(it.Position()));
				Assert::AreEqual(CellState::EMPTY, b2.At(it.Position()));
			}
		}

		TEST_METHOD(BoardFivesIterator) {
			// Test 1 piece
			{
				Board b(std::string() +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"*******R*******" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************");

				size_t totalCount{};
				size_t ones{};

				for (auto it = b.FivesBegin(); it != b.FivesEnd(); ++it) {
					++totalCount;
					if (*it == 1) ++ones;
				}

				Assert::AreEqual(static_cast<size_t>(5 * 4), ones, L"ones");
				Assert::AreEqual(static_cast<size_t>(15 * 11 * 2 + 11 * 11 * 2), totalCount, L"totalCount");
			}
			// Test triangle
			{
				Board b(std::string() +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"***************" +
					"*******B*******" +
					"*******BB******" +
					"***************" +
					"***************" +
					"***********R***" +
					"***************" +
					"***************" +
					"***************" +
					"***************");

				size_t minusTwos{};

				for (auto it = b.FivesBegin(); it != b.FivesEnd(); ++it) {
					if (*it == -2) ++minusTwos;
				}

				Assert::AreEqual(static_cast<size_t>(3 * 4 - 1), minusTwos, L"minusTwos");
			}
			// Test complicated
			{
				Board b(std::string() +
					"*B*****BB******" +
					"****B**********" +
					"****B**********" +
					"****B*****B****" +
					"***********B***" +
					"************B**" +
					"*************B*" +
					"***RRRRR******B" +
					"***************" +
					"***************" +
					"***R*******R***" +
					"***B******R****" +
					"***B*****R*****" +
					"***B****R******" +
					"***B***********");

				size_t fivesCount{};
				size_t foursCount{};
				size_t threesCount{};
				size_t twosCount{};
				size_t minusTwosCount{};
				size_t minusThreesCount{};
				size_t minusFoursCount{};
				size_t minusFivesCount{};

				for (auto it = b.FivesBegin(); it != b.FivesEnd(); ++it) {
					switch (*it) {
					case 5:
						++fivesCount;
						break;
					case 4:
						++foursCount;
						break;
					case 3:
						++threesCount;
						break;
					case 2:
						++twosCount;
						break;
					case -2:
						++minusTwosCount;
						break;
					case -3:
						++minusThreesCount;
						break;
					case -4:
						++minusFoursCount;
						break;
					case -5:
						++minusFivesCount;
						break;
					}
				}

				Assert::AreEqual(static_cast<size_t>(1), fivesCount, L"fivesCount");
				Assert::AreEqual(static_cast<size_t>(4), foursCount, L"foursCount");
				Assert::AreEqual(static_cast<size_t>(3), threesCount, L"threesCount");
				Assert::AreEqual(static_cast<size_t>(6), twosCount, L"twosCount");
				Assert::AreEqual(static_cast<size_t>(7), minusTwosCount, L"minusTwosCount");
				Assert::AreEqual(static_cast<size_t>(4), minusThreesCount, L"minusThreesCount");
				Assert::AreEqual(static_cast<size_t>(1), minusFoursCount, L"minusFoursCount");
				Assert::AreEqual(static_cast<size_t>(1), minusFivesCount, L"minusFivesCount");

			}
		}

		TEST_METHOD(BoardWins) {
			Board redWin(std::string() +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"****R**********" +
				"*****R*********" +
				"******R********" +
				"*******R*******" +
				"********R******" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************");

			Board blueWin(std::string() +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"**************B" +
				"**************B" +
				"**************B" +
				"**************B" +
				"**************B");

			Assert::IsTrue(redWin.RedWin());
			Assert::IsTrue(blueWin.BlueWin());
			Assert::IsFalse(redWin.BlueWin());
			Assert::IsFalse(blueWin.RedWin());
		}

		TEST_METHOD(MinimaxBehavior) {

			Board redWin(std::string() +
				"R**************" +
				"R**************" +
				"R**************" +
				"R**************" +
				"R**************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************");
			
			Board blueWin(std::string() +
				"BBBBB**********" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************");

			Board redWinsInOne(std::string() +
				"R**************" +
				"*R*************" +
				"**R************" +
				"***R***********" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************");

			// Testing base case with SimpleGoal

			constexpr Minimax<0, true, SimpleGoal> Test0{};

			Assert::AreEqual(1.0f, Test0(redWin), L"Test0: redWin");
			Assert::AreEqual(-1.0f, Test0(blueWin), L"Test0: blueWin");
			Assert::AreEqual(0.0f, Test0(redWinsInOne), L"Test0: redWinsInOne");

			// Testing ply depth of 1

			constexpr Minimax<1, true, SimpleGoal> Test1{};

			Assert::AreEqual(1.0f, Test1(redWinsInOne), L"Test1: Red should see a win.");

			// Testing ply depth of 2

			constexpr Minimax<2, false, SimpleGoal> Test2{};

			Assert::AreEqual(0.0f, Test2(redWinsInOne), L"Test2: Blue should see the block.");

			Board fourBsInARow(std::string() +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"******BBBB*****" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************");

			constexpr Minimax<2, true, SimpleGoal> Test3{};

			Assert::AreEqual(-1.0f, Test3(fourBsInARow), L"Test3: Red should see that blue wins.");			
		}

		TEST_METHOD(AlphaBetaPruning) {
			// This test is the same as for minimax, with alpha-beta pruning enabled

			Board redWin(std::string() +
				"R**************" +
				"R**************" +
				"R**************" +
				"R**************" +
				"R**************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************");

			Board blueWin(std::string() +
				"BBBBB**********" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************");

			Board redWinsInOne(std::string() +
				"R**************" +
				"*R*************" +
				"**R************" +
				"***R***********" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************");

			// Testing base case with SimpleGoal

			constexpr Minimax<0, true, SimpleGoal, true> Test0{};

			Assert::AreEqual(1.0f, Test0(redWin), L"Test0: redWin");
			Assert::AreEqual(-1.0f, Test0(blueWin), L"Test0: blueWin");
			Assert::AreEqual(0.0f, Test0(redWinsInOne), L"Test0: redWinsInOne");

			// Testing ply depth of 1

			constexpr Minimax<1, true, SimpleGoal, true> Test1{};

			Assert::AreEqual(1.0f, Test1(redWinsInOne), L"Test1: Red should see a win.");

			// Testing ply depth of 2

			constexpr Minimax<2, false, SimpleGoal, true> Test2{};

			Assert::AreEqual(0.0f, Test2(redWinsInOne), L"Test2: Blue should see the block.");

			Board fourBsInARow(std::string() +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"******BBBB*****" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************");

			constexpr Minimax<2, true, SimpleGoal, true> Test3{};

			Assert::AreEqual(-1.0f, Test3(fourBsInARow), L"Test3: Red should see that blue wins.");

			Board threeRsInARow(std::string() +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"*******R*******" +
				"******R********" +
				"*****R*********" +
				"***************" +
				"***************");

			constexpr Minimax<3, true, SimpleGoal, true> Test4{};
			constexpr Minimax<4, false, SimpleGoal, true> Test5{};
			Assert::AreEqual(1.0f, Test4(threeRsInARow), L"Test4: Red should see a win");
			Assert::AreEqual(0.0f, Test5(threeRsInARow), L"Test5: Blue should see two blocks.");
		}

		TEST_METHOD(ChildReturningMinimax) {
			
			// We'll test minimax and alpha-beta for simple cases, at depth 1, 
			// and alpha-beta again for the three in a row test
			
			Board redWinsInOne(std::string() +
				"R**************" +
				"*R*************" +
				"**R************" +
				"***R***********" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************");

			constexpr Minimax<1, true, SimpleGoal, false, true> Test0{};
			constexpr Minimax<1, true, SimpleGoal, true, true> Test1{};
			Assert::AreEqual(std::make_pair<size_t, size_t>(4, 4), Test0(redWinsInOne).Position(), L"Test0");
			Assert::AreEqual(std::make_pair<size_t, size_t>(4, 4), Test1(redWinsInOne).Position(), L"Test1");

			constexpr Minimax<2, false, SimpleGoal, false, true> Test2{};
			constexpr Minimax<2, false, SimpleGoal, true, true> Test3{};
			Assert::AreEqual(std::make_pair<size_t, size_t>(4, 4), Test2(redWinsInOne).Position(), L"Test2");
			Assert::AreEqual(std::make_pair<size_t, size_t>(4, 4), Test3(redWinsInOne).Position(), L"Test3");

			Board threeRsInARow(std::string() +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"*******R*******" +
				"******R********" +
				"*****R*********" +
				"***************" +
				"***************");

			constexpr Minimax<3, true, SimpleGoal, true, true> Test4{};
			auto result = Test4(threeRsInARow);
			Assert::IsTrue(std::make_pair<size_t, size_t>(4, 13) == result.Position() ||
				std::make_pair<size_t, size_t>(8, 9) == result.Position(),
				L"Test4: Red should see a win");
		}

		TEST_METHOD(RealSituations) {
			Board b1(std::string() +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"*********R*****" +
				"****B**RB******" +
				"*****BRBR******" +
				"****RBBBR******" +
				"*****RBBBR*****" +
				"******BRR******" +
				"******B********" +
				"******R********" +
				"***************" +
				"***************" +
				"***************");

			Board b2(std::string() +
				"***************" +
				"***************" +
				"***************" +
				"**R************" +
				"***R*****R*****" +
				"****B**RB******" +
				"*****BRBR******" +
				"****RBBBRB*****" +
				"***BBRBBBRBBBB*" +
				"****RBBRRBR****" +
				"****RRBBBRRR***" +
				"******RRRB*****" +
				"*******R*******" +
				"********B******" +
				"***************");

			Board b3(std::string() +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***B*R**RR*B***" +
				"****R**RBRB****" +
				"*****RBBBBRB***" +
				"******RBBBRB***" +
				"*******RRB*****" +
				"********BRR****" +
				"***************" +
				"***************" +
				"***************" +
				"***************");

			constexpr Minimax<4, true, GoalFunction, true, true> Goal{};
			Assert::AreEqual(std::make_pair<size_t, size_t>(3, 4), Goal(b1).Position(), L"Test: Red should block.");
			Assert::IsTrue(b2.Play(Goal(b2), false).RedWin(), L"Test: Red should win.");
			Assert::AreEqual(std::make_pair<size_t, size_t>(12, 4), Goal(b3).Position(), L"Test: Red should block.");

		}

		TEST_METHOD(GoalFunctionBehavior) {
			Board redWin(std::string() +
				"R**************" +
				"*R*************" +
				"**R************" +
				"***R***********" +
				"****R**********" +
				"***************" +
				"*******BBBB****" +
				"***************" +
				"***************" +
				"********BB*****" +
				"***************" +
				"******B********" +
				"*****B*********" +
				"****B**********" +
				"***B***********");

			Board blueWin(std::string() +
				"R**************" +
				"*R*************" +
				"**R************" +
				"***R***********" +
				"***************" +
				"***************" +
				"******BBBBB****" +
				"***************" +
				"***************" +
				"********BB*****" +
				"***************" +
				"******B********" +
				"*****B*********" +
				"****B**********" +
				"***B***********");

			Assert::AreEqual(std::numeric_limits<float>::infinity(), GoalFunction(redWin), L"redWin");
			Assert::AreEqual(-std::numeric_limits<float>::infinity(), GoalFunction(blueWin), L"blueWin");

			Board b(std::string() +
				"R**************" +		// Horizontal 1Rs: 1, 2, 3, 4, 8					   | 1Rs: 40
				"*R*************" +		// Vertical 1Rs: 1, 2, 3, 4, 1						   | 2Rs: 3
				"**R************" +		// SE 1Rs: 2										   | 1Bs: 131
				"***R***********" +		// SW 1Rs: 1, 3, 3, 2								   | 2Bs: 11
				"****B**********" +		// Vertical 2Rs: 3									   | 3Bs: 3
				"***************" +		// 													   | 4Bs: 4
				"*******BBBB****" +		// Horizontal 1Bs: 5, 2, 2, 5, 5, 5, 4				   |
				"***************" +		// Vertical 1Bs: 5, 5, 3, 3, 5, 3, 3, 4, 3, 2, 1	   | 40 + 3 * 4 
				"***************" +		// SE 1Bs: 1, 5, 5, 5, 5, 3, 2, 4, 3, 2				   | - 132 - 11 * 4
				"********BB*****" +		// SW 1Bs: 5, 5, 5, 5, 5, 2, 5						   | - 3 * 9
				"***************" +		// Horizontal 2Bs: 2, 4, Vertical 2Bs: 2, 2, SW 2Bs: 1 | - 4 * 16
				"******B****R***" +		// Horizontal 3Bs: 2, SW 3Bs: 1						   | = -215
				"*****B*****R***" +		// Horizontal 4Bs: 2, SW 4Bs: 2						   |
				"****B**********" +		//
				"***B***********");		//

			float score = GoalFunction(b);
			Assert::IsTrue(fabsf(-215 - score) < EPSILON, std::format(L"Expected: <{}>. Actual: <{}>", -174, score).c_str());
		}
	};
}

float SimpleGoal(const Board& board) {
	if (board.RedWin()) return 1.0f;
	if (board.BlueWin()) return -1.0f;
	return 0.0f;
}