#include "stdafx.h"
#include "CppUnitTest.h"
#include "..\ChessGame\BoardState.h"
#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

std::basic_string<wchar_t> ToString(SideType s)
{
	if (s == SideType::Black) return L"Black";
	if (s == SideType::White) return L"White";
	//Assert::Fail();
	return L"Null";
}

std::basic_string<wchar_t> ToString(PieceType p)
{
#define DEFINE_ENUM(__x) case PieceType::__x: return L#__x;
	switch (p)
	{
		DEFINE_ENUM(Empty)
		DEFINE_ENUM(Pawn)
		DEFINE_ENUM(Bishop)
		DEFINE_ENUM(Knight)
		DEFINE_ENUM(Rook)
		DEFINE_ENUM(Queen)
		DEFINE_ENUM(King)
	}
#undef DEFINE_ENUM
	return L"Unknown";
}

std::basic_string<wchar_t> ToString(const Piece& p)
{
	return ToString(p.Side) + L" " + ToString(p.Type);
}


namespace UnitTest
{		
	TEST_CLASS(BasicGameBoard)
	{
	public:
		
		TEST_METHOD(BoardSetup)
		{
			BoardState b;
			Assert::AreEqual(b.Get(0, 0).Side, SideType::Black);
			Assert::AreEqual(b.Get(0, 0).Type, PieceType::Rook);

			Assert::AreEqual(b.Get(1, 1).Side, SideType::Black);
			Assert::AreEqual(b.Get(1, 1).Type, PieceType::Pawn);

			Assert::AreEqual(b.Get(2, 2).Type, PieceType::Empty);

			Assert::AreEqual(b.Get(7, 7).Type, PieceType::Rook);
			Assert::AreEqual(b.Get(7, 7).Side, SideType::White);

			Assert::AreEqual(b.Get(6, 6).Type, PieceType::Pawn);
			Assert::AreEqual(b.Get(6, 6).Side, SideType::White);

			Assert::AreEqual(b.Get(2, 1), b.Get(3, 1));
			Assert::AreEqual(b.Get(2, 1), b.Get(4, 1));

			Assert::AreEqual(b.Get(4, 7), Piece(PieceType::King, SideType::White));
			Assert::AreEqual(b.Get(4, 0), Piece(PieceType::King, SideType::Black));		
		}


		TEST_METHOD(PawnMoves)
		{		
			BoardState b;

			//Assert::IsTrue(b.Move(4, 7, 4, 5));
			//Assert::AreEqual(Piece(PieceType::Pawn, SideType::White), b.Get(4,5));

			Assert::IsFalse(b.CanMove("e7", "e5"));
			Assert::IsFalse(b.CanMove("f7", "f5"));
			Assert::IsFalse(b.CanMove("g7", "g6"));

			Assert::IsFalse(b.CanMove("f2", "f5"), L"Pawn can't move 3 squares");
			Assert::IsTrue(b.CanMove("a2", "a4"));
			Assert::IsTrue(b.CanMove("b2", "b3"));

			Assert::IsTrue(b.Move("e2", "e4"), L"King pawn open");
			Assert::AreEqual(Piece(PieceType::Pawn, SideType::White), b.Get("e4"));

			// black move

			Assert::IsTrue(b.Move("e7", "e5"));

			Assert::IsTrue(b.Move("d2", "d4"));
			Assert::IsTrue(b.Move("e5", "d4"));
		}

	};
}