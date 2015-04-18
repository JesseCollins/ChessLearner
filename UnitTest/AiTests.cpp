#include "stdafx.h"
#include "CppUnitTest.h"
#include "BoardState.h"
#include "GameAi.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	TEST_CLASS(AiTests)
	{
	public:

		TEST_METHOD(CheckmateMatters)
		{
			BoardState b(
				"k     r "
				"        "
				"        "
				"        "
				"        "
				"     n  "
				"      p "
				"       K"
				, SideType::White);

			Assert::IsTrue(b.IsCheckmate());

			GameAi ai;
			auto score = ai.GetBoardScore(b);
			Assert::IsTrue(score < -1000000);
		}

		TEST_METHOD(CheckmateInOne)
		{
			BoardState b(
				"k     r "
				"        "
				"        "
				"        "
				"        "
				"     np "
				"        "
				"       K"
				, SideType::Black);

			Assert::IsFalse(b.IsCheckmate());

			GameAi ai;
			int score = 0;
			auto move = ai.DecideMoveImpl(b, 0, &score);

			Assert::IsTrue(score < -1000000);
			Assert::AreEqual(BoardLocation("g3"), move.From);
			Assert::AreEqual(BoardLocation("g2"), move.To);
		}

		TEST_METHOD(AvoidCheckmate)
		{
			BoardState b(
				"k   n   "
				"        "
				"        "
				"        "
				"        "
				"        "
				" R      "
				" R     K"
				, SideType::Black);

			Assert::IsFalse(b.IsCheckmate());

			GameAi ai;
			int score = 0;
			auto move = ai.DecideMoveImpl(b, 1, &score);

			Assert::AreEqual(BoardLocation("e8"), move.From);
			Assert::AreEqual(BoardLocation("c7"), move.To);
		}

		
		
		TEST_METHOD(ObviousCheckmate)
		{
			BoardState b(
				"k       "
				"      R "
				"       R"
				"        "
				"        "
				"        "
				"        "
				"       K"
				, SideType::White);

			GameAi ai;
			int score;
			//ai.StartDecideMove(b);
			//ai.WaitUntilFinished();

			//auto m = ai.GetMove();
			auto m1 = ai.DecideMoveImpl(b, 0, &score);


			Assert::IsTrue(score > 100000);
			Assert::AreEqual(BoardLocation("h8"), m1.To);
			Assert::AreEqual(BoardLocation("h6"), m1.From);
		}
		

	};
}