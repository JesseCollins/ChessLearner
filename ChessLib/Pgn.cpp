#include "stdafx.h"
#include "BoardState.h"

bool BoardState::MovePgn(const char* pgn)
{
	int size = strlen(pgn);
	const char* end = &pgn[size];

	BoardLocation from(InvalidBoardLocation);
	BoardLocation to(InvalidBoardLocation);
	bool isTaking = false;
	PieceType disambigPiece = PieceType::Pawn;
	int disambigRow = -1;
	int disambigCol = -1;
	bool isCheck = false;
	bool isCheckmate = false;

	if (pgn < end)
	{
		if (*(end - 1) == '+')
		{
			isCheck = true;
			--end;
		}
	}

	if (pgn < end)
	{
		if (*(end - 1) == '+')
		{
			isCheckmate = true;
			--end;
		}
	}

	// easy ones are easy
	if (strncmp(pgn, "O-O-O", 5) == 0)
	{
		disambigPiece = PieceType::King;
		to = BoardLocation(2, GetHomeRow(m_nextMoveSide));
		end = pgn;
	}
	if (strncmp(pgn, "O-O", 3) == 0)
	{
		disambigPiece = PieceType::King;
		to = BoardLocation(6, GetHomeRow(m_nextMoveSide));
		end = pgn;
	}

	// last bit should be a location
	if (pgn <= end - 2)
	{
		end -= 2;
		to = BoardLocation(end);
	}

	if (pgn < end)
	{
		if (*(end - 1) == 'x')
		{
			isTaking = true;
			--end;
		}
	}

	while (pgn < end)
	{
		--end;
		if (*end >= 'a' && *end <= 'h')
		{
			disambigCol = *end - 'a';
		}
		else if (*end >= '1' && *end <= '8')
		{
			disambigRow = *end - '1';
		}
		else
		{
			disambigPiece = GetPieceType(*end);
			if (disambigPiece == PieceType::Invalid)
			{
				throw "char unexpected";
				return false;
			}
		}
	}

	// OK now we've gone through the whole string and think we have what we need!
	// Look through the clues.

	for (auto loc : *this)
	{
		if (disambigCol >= 0 && disambigCol != loc.X()) continue;
		if (disambigRow >= 0 && disambigRow != loc.Y()) continue;
		if (CanMove(loc, to))
		{
			if (Get(loc).Type != disambigPiece) continue;
			if (from != InvalidBoardLocation)
			{
				// Ambiguous!!!
				throw "Needs more disambiguation";
				return false;
			}
			from = loc;
		}
	}

	if (from == InvalidBoardLocation || to == InvalidBoardLocation)
	{
		throw "Missed something";
		return false;
	}

	if (isTaking && Get(to).Type == PieceType::Empty)
	{
		throw "No piece there";
	}
	return Move(from, to);
}
