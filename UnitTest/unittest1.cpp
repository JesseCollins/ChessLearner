#include "stdafx.h"
#include "CppUnitTest.h"
#include "BoardState.h"
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
		
		TEST_METHOD(PerfBasics)
		{
			BoardState b;
			Assert::IsTrue(sizeof(b) <= 64);
		}

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

		TEST_METHOD(PgnMove)
		{
			BoardState b;

			Assert::IsTrue(b.MovePgn("e4"));

			Assert::AreEqual(Piece(PieceType::Pawn, SideType::White), b.Get("e4"));


		}

		TEST_METHOD(FullPgnGame)
		{
			BoardState b;
			Assert::IsTrue(b.MovePgn("e4"));
			Assert::IsTrue(b.MovePgn("e5"));
			Assert::IsTrue(b.MovePgn("Nf3"));
			Assert::IsTrue(b.MovePgn("Nc6"));
			Assert::IsTrue(b.MovePgn("Bb5"));
			Assert::IsTrue(b.MovePgn("a6"));
			Assert::IsTrue(b.MovePgn("Ba4"));
			Assert::IsTrue(b.MovePgn("Nf6"));
			Assert::IsTrue(b.MovePgn("O-O"));
			Assert::IsTrue(b.MovePgn("Be7"));
			Assert::IsTrue(b.MovePgn("Re1"));
			Assert::IsTrue(b.MovePgn("b5"));
			Assert::IsTrue(b.MovePgn("Bb3"));
			Assert::IsTrue(b.MovePgn("d6"));
			Assert::IsTrue(b.MovePgn("c3"));
			Assert::IsTrue(b.MovePgn("O-O"));
			Assert::IsTrue(b.MovePgn("h3"));
			Assert::IsTrue(b.MovePgn("Nb8"));
			Assert::IsTrue(b.MovePgn("d4"));
			Assert::IsTrue(b.MovePgn("Nbd7"));
			Assert::IsTrue(b.MovePgn("c4"));
			Assert::IsTrue(b.MovePgn("c6"));
			Assert::IsTrue(b.MovePgn("cxb5"));
			Assert::IsTrue(b.MovePgn("axb5"));
			Assert::IsTrue(b.MovePgn("Nc3"));
			Assert::IsTrue(b.MovePgn("Bb7"));
			Assert::IsTrue(b.MovePgn("Bg5"));
			Assert::IsTrue(b.MovePgn("b4"));
			Assert::IsTrue(b.MovePgn("Nb1"));
			Assert::IsTrue(b.MovePgn("h6"));
			Assert::IsTrue(b.MovePgn("Bh4"));
			Assert::IsTrue(b.MovePgn("c5"));
			Assert::IsTrue(b.MovePgn("dxe5"));
			Assert::IsTrue(b.MovePgn("Nxe4"));
			Assert::IsTrue(b.MovePgn("Bxe7"));
			Assert::IsTrue(b.MovePgn("Qxe7"));
			Assert::IsTrue(b.MovePgn("exd6"));
			Assert::IsTrue(b.MovePgn("Qf6"));
			Assert::IsTrue(b.MovePgn("Nbd2"));
			Assert::IsTrue(b.MovePgn("Nxd6"));
			Assert::IsTrue(b.MovePgn("Nc4"));
			Assert::IsTrue(b.MovePgn("Nxc4"));
			Assert::IsTrue(b.MovePgn("Bxc4"));
			Assert::IsTrue(b.MovePgn("Nb6"));
			Assert::IsTrue(b.MovePgn("Ne5"));
			Assert::IsTrue(b.MovePgn("Rae8"));
			Assert::IsTrue(b.MovePgn("Bxf7+"));
			Assert::IsTrue(b.MovePgn("Rxf7"));
			Assert::IsTrue(b.MovePgn("Nxf7"));
			Assert::IsTrue(b.MovePgn("Rxe1+"));
			Assert::IsTrue(b.MovePgn("Qxe1"));
			Assert::IsTrue(b.MovePgn("Kxf7"));
			Assert::IsTrue(b.MovePgn("Qe3"));
			Assert::IsTrue(b.MovePgn("Qg5"));
			Assert::IsTrue(b.MovePgn("Qxg5"));
			Assert::IsTrue(b.MovePgn("hxg5"));
			Assert::IsTrue(b.MovePgn("b3"));
			Assert::IsTrue(b.MovePgn("Ke6"));
			Assert::IsTrue(b.MovePgn("a3"));
			Assert::IsTrue(b.MovePgn("Kd6"));
			Assert::IsTrue(b.MovePgn("axb4"));
			Assert::IsTrue(b.MovePgn("cxb4"));
			Assert::IsTrue(b.MovePgn("Ra5"));
			Assert::IsTrue(b.MovePgn("Nd5"));
			Assert::IsTrue(b.MovePgn("f3"));
			Assert::IsTrue(b.MovePgn("Bc8"));
			Assert::IsTrue(b.MovePgn("Kf2"));
			Assert::IsTrue(b.MovePgn("Bf5"));
			Assert::IsTrue(b.MovePgn("Ra7"));
			Assert::IsTrue(b.MovePgn("g6"));
			Assert::IsTrue(b.MovePgn("Ra6+"));
			Assert::IsTrue(b.MovePgn("Kc5"));
			Assert::IsTrue(b.MovePgn("Ke1"));
			Assert::IsTrue(b.MovePgn("Nf4"));
			Assert::IsTrue(b.MovePgn("g3"));
			Assert::IsTrue(b.MovePgn("Nxh3"));
			Assert::IsTrue(b.MovePgn("Kd2"));
			Assert::IsTrue(b.MovePgn("Kb5"));
			Assert::IsTrue(b.MovePgn("Rd6"));
			Assert::IsTrue(b.MovePgn("Kc5"));
			Assert::IsTrue(b.MovePgn("Ra6"));
			Assert::IsTrue(b.MovePgn("Nf2"));
			Assert::IsTrue(b.MovePgn("g4"));
			Assert::IsTrue(b.MovePgn("Bd3"));
			Assert::IsTrue(b.MovePgn("Re6"));
		}

		TEST_METHOD(SimpleCastle)
		{
			BoardState b(
				"k       "
				"p       "
				"        "
				"        "
				"        "
				"        "
				"        "
				"R   K  R"
				, SideType::White);

			Assert::AreEqual(Piece(PieceType::King, SideType::White), b.Get("e1"));
			Assert::AreEqual(Piece(PieceType::Rook, SideType::White), b.Get("h1"));
			
			auto b2 = b;
			Assert::IsTrue(b2.Move("e1", "g1", nullptr));		

			b2 = b;
			Assert::IsTrue(b2.Move("e1", "c1", nullptr));
		}

		void AssertCantCastle(const BoardState& b)
		{
			auto local = b;
			Assert::IsFalse(local.Move("e1", "g1", nullptr));
			Assert::IsFalse(local.Move("e1", "c1", nullptr));
		}

		TEST_METHOD(CastlingNeedsRook)
		{			
			AssertCantCastle(BoardState(
				"k       "
				"p       "
				"        "
				"        "
				"        "
				"        "
				"        "
				"    K   "
				, SideType::White));

			AssertCantCastle(BoardState(
				"k       "
				"p       "
				"        "
				"        "
				"        "
				"        "
				"        "
				"RQ  K   "
				, SideType::White));
		}

		TEST_METHOD(CantCastleThroughCheck)
		{
			AssertCantCastle(BoardState(
				"k       "
				"p       "
				"        "
				"     r  "
				"        "
				"        "
				"        "
				"    K  R"
				, SideType::White));

			AssertCantCastle(BoardState(
				"k       "
				"p       "
				"        "
				"    r   "
				"        "
				"        "
				"        "
				"    K  R"
				, SideType::White));
		}

		TEST_METHOD(CantCastleAfterKingMoved)
		{
			BoardState b(
				"k       "
				"p       "
				"        "
				"        "
				"        "
				"        "
				"        "
				"R    K R"
				, SideType::White);

			Assert::IsTrue(b.Move("f1", "e1"));
			Assert::IsTrue(b.Move("a7", "a5"));
			AssertCantCastle(b);
		}

		TEST_METHOD(CantCastleAfterRookMoved)
		{
			BoardState b(
				"k       "
				"p       "
				"        "
				"        "
				"        "
				"        "
				"        "
				"    K  R"
				, SideType::White);

			Assert::IsTrue(b.Move("h1", "h2"));
			Assert::IsTrue(b.Move("a7", "a5"));
			Assert::IsTrue(b.Move("h2", "h1"));
			Assert::IsTrue(b.Move("a5", "a4"));
			AssertCantCastle(b);
		}

		TEST_METHOD(RegressPawnCantJumpPieces)
		{
			BoardState b(
				"k       "
				"p       "
				"P       "
				"        "
				"        "
				"        "
				"        "
				"       K"
				, SideType::Black);

			Assert::IsFalse(b.Move("a7", "a5"));
		}
	};
}