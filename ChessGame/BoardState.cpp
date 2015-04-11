#include "stdafx.h"
#include "BoardState.h"

extern const BoardLocation InvalidBoardLocation(64);

BoardState::BoardState(const char* board, SideType nextMove)
	: m_nextMoveSide(nextMove)
	, m_enPassantCol(-1)
{
	for (int y = 0; y < 8; ++y)
	{
		for (int x = 0; x < 8; ++x)
		{
			auto p = GetPieceAndSide(*board);
			Set({ x, y }, p);
			board++;
		}		
	}

}




PieceType GetPieceType(const char ch)
{
	auto upper = ::toupper(ch);
	for (int i = 1; i < sizeof(PieceTypeChars); ++i)
	{
		if (PieceTypeChars[i] == upper)
		{
			return static_cast<PieceType>(i);
		}
	}
	return PieceType::Empty;
}

Piece GetPieceAndSide(const char c)
{
	return Piece(
		GetPieceType(c),
		static_cast<SideType>(!!::islower(c)));
}


int GetHomeRow(SideType side)
{
	switch (side)
	{
		case SideType::Black: return 0;
		case SideType::White: return 7;
	}
	throw "Unexpected home row!";
	return -1;
}

int GetEnPassantRow(SideType side)
{
	switch (side)
	{
	case SideType::White: return 2;
	case SideType::Black: return 5;
	}
	throw "Unexpected side!";
	return -1;
}


BoardState::MoveCollection BoardState::ValidMoves() const
{
	MoveCollection collection;

	for (auto from : *this)
	{
		for (auto to : *this)
		{
			if (CanMove(from, to))
			{
				collection.push_back({ from, to });
			}
		}
	}
	return collection;
}

