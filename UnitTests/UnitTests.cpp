#include "pch.h"
#include "UnitTests.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace Constants;

// Returns 1.0f if red won, -1.0f if blue, and 0.0f if neither has won.
float SimpleGoal(const Board&);

// An allowed error, for floating point comparisons
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

		TEST_METHOD(BoardFullAndEmpty) {
			Board neither(std::string() +
				"B**************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***********B***" +
				"**********R****" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***************" +
				"***R***********" +
				"***************");

			Board full(std::string() +
				"BRBRBRBRBRBRBRB" +
				"RBRBRBRBRBRBRBR" +
				"BRBRBRBRBRBRBRB" +
				"RBRBRBRBRBRBRBR" +
				"BRBRBRBRBRBRBRB" +
				"RBRBRBRBRBRBRBR" +
				"BRBRBRBRBRBRBRB" +
				"RBRBRBRBRBRBRBR" +
				"BRBRBRBRBRBRBRB" +
				"RBRBRBRBRBRBRBR" +
				"BRBRBRBRBRBRBRB" +
				"RBRBRBRBRBRBRBR" +
				"BRBRBRBRBRBRBRB" +
				"RBRBRBRBRBRBRBR" +
				"BRBRBRBRBRBRBRB");

			Assert::IsTrue(Board().Empty());
			Assert::IsFalse(Board().Full());
			Assert::IsFalse(neither.Empty());
			Assert::IsFalse(neither.Full());
			Assert::IsFalse(full.Empty());
			Assert::IsTrue(full.Full());



		}
		TEST_METHOD(BoardInRangePlies) {
			if constexpr (RANGE == 1)
			{
				Board b(std::string() +
					"B**************" +			//B *************		    1
					"***************" +			//  *************			2		  
					"***************" +			//*************** 			0
					"***************" +			//*************** 			0
					"***************" +			//*************** 			0			 
					"***************" +			//**********   **			3			 
					"***********B***" +			//*********  B **  			3
					"**********R****" +			//********* R  **  			3
					"***************" +			//*********   ***  			3
					"***************" +			//***************			0
					"***************" +			//***************			0
					"***************" +			//***************			0
					"***************" +			//**   **********			3
					"***R***********" +			//** R **********			2
					"***************");			//**   **********			3

				size_t count{};

				size_t counts[15]{};

				auto plies = b.InRangePlies();

				for (auto ply : plies) {
					++count;
					Assert::AreEqual(CellState::EMPTY, b.At(ply));
					const auto x = ply % BOARD_WIDTH;
					const auto y = ply / BOARD_WIDTH;
					++counts[y];

					switch (y) {
					case 0:
						Assert::IsTrue(x == 1, std::format(L"x = {}, y = {}", x, y).c_str());
						break;
					case 1:
						Assert::IsTrue(x <= 1, std::format(L"x = {}, y = {}", x, y).c_str());
						break;
					case 2: case 3: case 4:
						Assert::IsTrue(false, std::format(L"x = {}, y = {}", x, y).c_str());
						break;
					case 5:
						Assert::IsTrue(10 <= x && x <= 12, std::format(L"x = {}, y = {}", x, y).c_str());
						break;
					case 6:
						Assert::IsTrue(x == 9 || x == 10 || x == 12, std::format(L"x = {}, y = {}", x, y).c_str());
						break;
					case 7:
						Assert::IsTrue(x == 9 || x == 11 || x == 12, std::format(L"x = {}, y = {}", x, y).c_str());
						break;
					case 8:
						Assert::IsTrue(9 <= x && x <= 11, std::format(L"x = {}, y = {}", x, y).c_str());
						break;
					case 9: case 10: case 11:
						Assert::IsTrue(false, std::format(L"x = {}, y = {}", x, y).c_str());
						break;
					case 12: case 14:
						Assert::IsTrue(2 <= x && x <= 4, std::format(L"x = {}, y = {}", x, y).c_str());
						break;
					case 13:
						Assert::IsTrue(x == 2 || x == 4, std::format(L"x = {}, y = {}", x, y).c_str());
						break;
					}
				}
				Assert::AreEqual(static_cast<size_t>(1), counts[0], L"y = 0");
				Assert::AreEqual(static_cast<size_t>(2), counts[1], L"y = 1");
				Assert::AreEqual(static_cast<size_t>(0), counts[2], L"y = 2");
				Assert::AreEqual(static_cast<size_t>(0), counts[3], L"y = 3");
				Assert::AreEqual(static_cast<size_t>(0), counts[4], L"y = 4");
				Assert::AreEqual(static_cast<size_t>(3), counts[5], L"y = 5");
				Assert::AreEqual(static_cast<size_t>(3), counts[6], L"y = 6");
				Assert::AreEqual(static_cast<size_t>(3), counts[7], L"y = 7");
				Assert::AreEqual(static_cast<size_t>(3), counts[8], L"y = 8");
				Assert::AreEqual(static_cast<size_t>(0), counts[9], L"y = 9");
				Assert::AreEqual(static_cast<size_t>(0), counts[10], L"y = 10");
				Assert::AreEqual(static_cast<size_t>(0), counts[11], L"y = 11");
				Assert::AreEqual(static_cast<size_t>(3), counts[12], L"y = 12");
				Assert::AreEqual(static_cast<size_t>(2), counts[13], L"y = 13");
				Assert::AreEqual(static_cast<size_t>(3), counts[14], L"y = 14");
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

			Assert::AreEqual(static_cast<size_t>(0), full.InRangePlies().size());
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

			for (size_t i = 0; i < BOARD_SIZE; ++i) {
				if (b2.At(i) != CellState::EMPTY) continue;
				Board b3 = b2.Play(i, pos % 2);
				Assert::AreEqual((pos % 2) ? CellState::BLUE : CellState::RED, b3.At(i));
				Assert::AreEqual(CellState::EMPTY, b2.At(i));
			}
		}

		TEST_METHOD(FivesArrays) {
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

				size_t totalHorizontalCount{};
				size_t horizontalOnes{};
				size_t totalVerticalCount{};
				size_t verticalOnes{};
				size_t totalSoutheastCount{};
				size_t southeastOnes{};
				size_t totalSouthwestCount{};
				size_t southwestOnes{};

				for (auto root : HORIZONTAL_FIVES_ROOTS) {
					++totalHorizontalCount;
					if (b.CountFive<FivesOrientation::HORIZONTAL>(root) == 1) ++horizontalOnes;
				}
				for (auto root : VERTICAL_FIVES_ROOTS) {
					++totalVerticalCount;
					if (b.CountFive<FivesOrientation::VERTICAL>(root) == 1) ++verticalOnes;
				}
				for (auto root : SOUTHEAST_FIVES_ROOTS) {
					++totalSoutheastCount;
					if (b.CountFive<FivesOrientation::SOUTHEAST>(root) == 1) ++southeastOnes;
				}
				for (auto root : SOUTHWEST_FIVES_ROOTS) {
					++totalSouthwestCount;
					if (b.CountFive<FivesOrientation::SOUTHWEST>(root) == 1) ++southwestOnes;
				}

				Assert::AreEqual(static_cast<size_t>(5), horizontalOnes, L"horizontalOnes");
				Assert::AreEqual(static_cast<size_t>(15 * 11), totalHorizontalCount, L"totalHorizontalCount");
				Assert::AreEqual(static_cast<size_t>(5), verticalOnes, L"verticalOnes");
				Assert::AreEqual(static_cast<size_t>(15 * 11), totalVerticalCount, L"totalVerticalCount");
				Assert::AreEqual(static_cast<size_t>(5), southeastOnes, L"southeastOnes");
				Assert::AreEqual(static_cast<size_t>(11 * 11), totalSoutheastCount, L"totalSoutheastCount");
				Assert::AreEqual(static_cast<size_t>(5), southwestOnes, L"southwestOnes");
				Assert::AreEqual(static_cast<size_t>(11 * 11), totalSouthwestCount, L"totalSouthwestCount");


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

				for (auto root : HORIZONTAL_FIVES_ROOTS) {
					if (b.CountFive<FivesOrientation::HORIZONTAL>(root) == -2) ++minusTwos;
				}
				for (auto root : VERTICAL_FIVES_ROOTS) {
					if (b.CountFive<FivesOrientation::VERTICAL>(root) == -2) ++minusTwos;
				}
				for (auto root : SOUTHEAST_FIVES_ROOTS) {
					if (b.CountFive<FivesOrientation::SOUTHEAST>(root) == -2) ++minusTwos;
				}
				for (auto root : SOUTHWEST_FIVES_ROOTS) {
					if (b.CountFive<FivesOrientation::SOUTHWEST>(root) == -2) ++minusTwos;
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

				auto countUp = [&](const auto count) {
					switch (count) {
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
				};

				for (auto root : HORIZONTAL_FIVES_ROOTS) {
					countUp(b.CountFive<FivesOrientation::HORIZONTAL>(root));
				}
				for (auto root : VERTICAL_FIVES_ROOTS) {
					countUp(b.CountFive<FivesOrientation::VERTICAL>(root));
				}
				for (auto root : SOUTHEAST_FIVES_ROOTS) {
					countUp(b.CountFive<FivesOrientation::SOUTHEAST>(root));
				}
				for (auto root : SOUTHWEST_FIVES_ROOTS) {
					countUp(b.CountFive<FivesOrientation::SOUTHWEST>(root));
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

			constexpr Minimax<3, true, SimpleGoal> Test4{};
			constexpr Minimax<4, false, SimpleGoal> Test5{};
			Assert::AreEqual(1.0f, Test4(threeRsInARow), L"Test4: Red should see a win");
			Assert::AreEqual(0.0f, Test5(threeRsInARow), L"Test5: Blue should see two blocks.");
		}

		TEST_METHOD(ChildReturningMinimax) {

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

			constexpr Minimax<1, true, SimpleGoal, true> Test0{};
			constexpr Minimax<1, true, SimpleGoal, true> Test1{};
			Assert::AreEqual(4 * BOARD_WIDTH + 4, Test0(redWinsInOne), L"Test0");
			Assert::AreEqual(4 * BOARD_WIDTH + 4, Test1(redWinsInOne), L"Test1");

			constexpr Minimax<2, false, SimpleGoal, true> Test2{};
			constexpr Minimax<2, false, SimpleGoal, true> Test3{};
			Assert::AreEqual(4 * BOARD_WIDTH + 4, Test2(redWinsInOne), L"Test2");
			Assert::AreEqual(4 * BOARD_WIDTH + 4, Test3(redWinsInOne), L"Test3");

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
			auto result = Test4(threeRsInARow);
			Assert::IsTrue(13 * BOARD_WIDTH + 4 == result ||
				9 * BOARD_WIDTH + 8 == result,
				L"Test4: Red should see a win");
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

			float actual = GoalFunction(b);
			float expected = 40 * SCORE_MAP[0] +
				3 * SCORE_MAP[1] +
				132 * -SCORE_MAP[0] +
				11 * -SCORE_MAP[1] +
				3 * -SCORE_MAP[2] +
				4 * -SCORE_MAP[3];
			Assert::IsTrue(fabsf(expected - actual) < EPSILON, std::format(L"Expected: <{}>. Actual: <{}>", expected, actual).c_str());
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

			constexpr Minimax<PLY_LOOK_AHEAD, true, GoalFunction, true> Goal{};
			Assert::AreEqual(4 * BOARD_WIDTH + 3, Goal(b1), L"Test: Red should block.");
			Assert::IsTrue(b2.Play(Goal(b2), false).RedWin(), L"Test: Red should win.");
			Assert::AreEqual(4 * BOARD_WIDTH + 12, Goal(b3), L"Test: Red should block.");
		}

		TEST_CLASS_CLEANUP(KillThreads) {
			GoalFunctionThreadPool::Kill();
		}
	};
}

float SimpleGoal(const Board& board) {
	if (board.RedWin()) return 1.0f;
	if (board.BlueWin()) return -1.0f;
	return 0.0f;
}