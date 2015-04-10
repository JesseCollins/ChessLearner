#include "stdafx.h"
#include "BoardState.h"

extern const BoardLocation InvalidBoardLocation(64);


PieceType GetPieceType(const char c)
{
	for (int i = 1; i < sizeof(PieceTypeChars); ++i)
	{
		if (PieceTypeChars[i] == c)
		{
			return static_cast<PieceType>(i);
		}
	}
	return PieceType::Invalid;
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


enum class PgnExprType
{
	End,
	PieceType,
	Location,
	Takes,
	Equals,
	KingSideCastle,
	QueenSideCastle,
	Invalid
};

struct PgnExpression
{
	PgnExprType Type;
	const char* Start;
	const char* End;
};


PgnExpression GetNextPgnExpression(const char* pgn)
{
	PgnExpression expr;
	expr.Start = pgn;
	expr.End = pgn;

	if (*pgn == '\0')
	{
		expr.Type = PgnExprType::End;
		return expr;
	}
	if (pgn[0] == 'x')
	{
		expr.Type = PgnExprType::Takes;
		return expr;
	}
	if (pgn[0] >= 'a' && pgn[0] <= 'h'
		&& pgn[1] >= '1' && pgn[1] <= '8')
	{
		expr.End = &pgn[1];
		expr.Type = PgnExprType::Location;
		return expr;
	}	
	for (auto type : PieceTypeChars)
	{
		if (pgn[0] == type)
		{
			expr.Type = PgnExprType::PieceType;
			return expr;
		}
	}

	expr.Type = PgnExprType::Invalid;
	return expr;
}


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

#if 0
bool BoardState::MovePgn(const char* pgn)
{
	int size = strlen(pgn);
	/*
	if (size == 2)
	{
		// Must be a simple pawn move
		auto to = BoardLocation(pgn);

		for (auto loc : *this)
		{
			if (this->CanMove(loc, to))
			{
				Move(loc, to);
				return true;
			}
		}
	}
	*/

	PieceType movingPieceType = PieceType::Invalid;
	BoardLocation from(InvalidBoardLocation);
	BoardLocation to(InvalidBoardLocation);

	PgnExpression expr;
	do
	{
		expr = GetNextPgnExpression(pgn);

		if (expr.Type == PgnExprType::Location)
		{
			BoardLocation loc(pgn);
			if (movingPieceType == PieceType::Invalid && from == InvalidBoardLocation)
			{
				if (Get(BoardLocation(pgn)).Side == m_nextMoveSide)
				{
					// must be moving *from* this square
					from = BoardLocation(pgn);
				}
				else
				{
					// This is a location but we don't know what piece yet.
					// Must be a pawn move!
					to = BoardLocation(pgn);
					BoardLocation fromCandidate(InvalidBoardLocation);
					for (auto loc : *this)
					{
						if (this->CanMove(loc, to))
						{
							if (fromCandidate == InvalidBoardLocation)
							{
								fromCandidate = loc;
							}
							else
							{
								// ambiguous!
								return false;
							}
						}
					}
					if (fromCandidate == InvalidBoardLocation)
					{
						// couldn't find candidate pawn!
						return false;
					}
					from = fromCandidate;
				}
			}
			else
			{
				// We know the piece type.  This could be a destination or
				// a disambiguator...			
				auto peekExpr = GetNextPgnExpression(expr.End + 1);
				const bool isDisambiguator =
					peekExpr.Type == PgnExprType::Location ||
					peekExpr.Type == PgnExprType::Equals ||
					peekExpr.Type == PgnExprType::Takes;

				if (isDisambiguator)
				{
					if (from != InvalidBoardLocation)
					{
						return false;
					}
					from = loc;
				}
				else
				{
					to = loc;
				}
			}
		}
		else if (expr.Type == PgnExprType::PieceType)
		{
			movingPieceType = GetPieceType(pgn[0]);

		}


		pgn = expr.End + 1;
	}
	while (expr.Type != PgnExprType::End);


	// Couldn't figure out what to do!
	return false;
}

#endif