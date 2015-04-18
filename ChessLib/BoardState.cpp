#include "stdafx.h"
#include "BoardState.h"

extern const BoardLocation InvalidBoardLocation(64);
extern const ChessMove InvalidChessMove({ InvalidBoardLocation, InvalidBoardLocation });

extern int g_canMoveCalls = 0;
extern int g_boardScoreCalls = 0;

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
	InitializeKingPositions();
}

void BoardState::InitializeKingPositions()
{
	for (int y = 0; y < 8; ++y)
	{
		for (int x = 0; x < 8; ++x)
		{
			auto loc = BoardLocation(x, y);
			auto p = Get(loc);
			if (p.Type == PieceType::King)
			{
				m_kingPosition[static_cast<int>(p.Side)] = loc;
			}
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
		auto fromPiece = Get(from);
		if (fromPiece.Type != PieceType::Empty && fromPiece.Side == m_nextMoveSide)
		{
			for (auto to : *this)
			{
				if (CanMove(from, to))
				{
					collection.push_back({ from, to });
				}
			}
		}
	}
	return collection;
}


bool BoardState::CanMove(BoardLocation from, BoardLocation to) const
{
	++g_canMoveCalls;

	auto fromPiece = Get(from);
	auto toPiece = Get(to);

	// There's no piece there
	if (fromPiece.Type == PieceType::Empty) return false;

	// It's not your turn
	if (fromPiece.Side != m_nextMoveSide) return false;

	// Can't capture own piece (also covers moving to same square)
	if (toPiece.Type != PieceType::Empty && toPiece.Side == fromPiece.Side) return false;

	const int xdist = abs(to.X() - from.X());
	const int ydist = abs(to.Y() - from.Y());

	if (fromPiece.Type == PieceType::Pawn)
	{
		int direction = (m_nextMoveSide == SideType::White) ? -1 : 1;
		int startRow = (m_nextMoveSide == SideType::White) ? 6 : 1;

		// Move correct direction
		if (Sign(to.Y() - from.Y()) != direction) return false;

		// Max 2 squares
		if (ydist > 2) return false;

		// 2 squares only ok from start position
		if (ydist == 2 && from.Y() != startRow) return false;

		// Capture
		if (from.X() != to.X())
		{
			// Must be one square left or right
			if (abs(from.X() - to.X()) != 1) return false;

			// Must be one square in correct direction
			if (ydist != 1) return false;

			// en passant?
			if (to.X() == m_enPassantCol && to.Y() == GetEnPassantRow(m_nextMoveSide))
			{
				// that's en passant
			}
			else
			{
				// Must capture
				if (toPiece.Type == PieceType::Empty) return false;
			}
		}
		else
		{
			// non-diagonal movements can't capture
			if (toPiece.Type != PieceType::Empty) return false;
		}

		// Ensure unobstructed for 2-square moves
		if (IsObstructed(from, to)) return false;
	}
	else if (fromPiece.Type == PieceType::Bishop)
	{
		if (xdist != ydist) return false;
		if (IsObstructed(from, to)) return false;
	}
	else if (fromPiece.Type == PieceType::Knight)
	{
		if (xdist + ydist != 3) return false;
		if (xdist == 0 || ydist == 0) return false;
	}
	else if (fromPiece.Type == PieceType::Rook)
	{
		if (xdist != 0 && ydist != 0) return false;
		if (IsObstructed(from, to)) return false;
	}
	else if (fromPiece.Type == PieceType::Queen)
	{
		const bool movingLikeRook = xdist == 0 || ydist == 0;
		const bool movingLikeBishop = xdist == ydist;
		if (!movingLikeBishop && !movingLikeRook) return false;
		if (IsObstructed(from, to)) return false;
	}
	else if (fromPiece.Type == PieceType::King)
	{
		// castling!  hacky, should this be done differently?
		if (GetHomeRow(m_nextMoveSide) == from.Y() &&
			ydist == 0 && from.X() == 4 && (to.X() == 2 || to.X() == 6))
		{
			if (!CanCastle(from, to)) return false;
		}
		else
		{
			if (xdist > 1 || ydist > 1) return false;
		}
	}
	else
	{
		assert(false);
		return false;
	}

	// Now the fancy stuff.  Can't move into check.
	// If this hypothetical move is taking a king, it's always ideal!
	if (toPiece.Type != PieceType::King)
	{
		auto newState = *this;
		newState.Move(from, to, true);
		if (newState.CanTakeKing())
		{
			// If move is allowed, king can be taken
			return false;
		}
	}

	return true;
}


bool BoardState::CanCastle(BoardLocation from, BoardLocation to) const
{
	if (IsCheck())
	{
		return false;
	}

	// Some math to avoid branching.  
	const auto offset = static_cast<int>(m_nextMoveSide)* 3;
	if (m_hasPieceMoved.test(offset))
	{
		return false;
	}
	if (m_hasPieceMoved.test(offset + to.X() / 5 + 1))
	{
		return false;
	}	

	const auto direction = Sign(to.X() - from.X());
	const auto rookDirection = -direction;

	// Rook must be there...
	BoardLocation rookLocation(7, from.Y());
	if (to.X() < from.X())
	{
		rookLocation = BoardLocation(0, from.Y());
	}	
	if (Get(rookLocation) != Piece(PieceType::Rook, m_nextMoveSide))
	{
		return false;
	}

	// ...and unobstructed
	if (Get(rookLocation.X() + rookDirection, from.Y()).Type != PieceType::Empty)
	{
		return false;
	}

	// Can't castle through check
	auto intermediate = BoardLocation((from.X() + to.X()) / 2, from.Y());
	if (!CanMove(from, intermediate))
	{
		return false;
	}

	return true;
}


bool BoardState::MoveImpl(BoardLocation from, BoardLocation to, MoveCallback callback)
{
	auto movingPiece = Get(from);

	if (Get(from).Type == PieceType::King)
	{
		if (from.X() + 2 == to.X())
		{
			MovePiece(BoardLocation(7, from.Y()), BoardLocation(5, from.Y()), callback);
		}
		if (from.X() - 2 == to.X())
		{
			MovePiece(BoardLocation(0, from.Y()), BoardLocation(3, from.Y()), callback);
		}
	}

	if (Get(from).Type == PieceType::Pawn
		&& from.X() != to.X()
		&& Get(to).Type == PieceType::Empty)
	{
		// en passant -- remove the victim
		auto victimLoc = BoardLocation(m_enPassantCol, from.Y());
		MovePiece(victimLoc, InvalidBoardLocation, callback);
	}

	MovePiece(from, to, callback);

	// Update state

	if (movingPiece.Type == PieceType::Pawn && abs(from.Y() - to.Y()) == 2)
	{
		m_enPassantCol = from.X();
	}
	else
	{
		m_enPassantCol = -1;
	}

	if (movingPiece.Type == PieceType::King)
	{
		const int sideOffset = static_cast<int>(m_nextMoveSide)* 3;
		m_hasPieceMoved.set(sideOffset);

		m_kingPosition[static_cast<int>(m_nextMoveSide)] = to;
	}

	if (movingPiece.Type == PieceType::Rook && from.Y() == GetHomeRow(m_nextMoveSide))
	{
		if (from.X() == 0 || from.X() == 7)
		{
			// Rook moving from starting pos, make sure to remember it moved
			const int sideOffset = static_cast<int>(m_nextMoveSide)* 3;
			m_hasPieceMoved.set(sideOffset + 1 + (from.X() / 7));
		}
	}

	this->m_nextMoveSide = m_nextMoveSide == SideType::White ? SideType::Black : SideType::White;

	return true;
}

bool BoardState::CanTakeKing() const
{	
	BoardLocation kingLoc(m_kingPosition[static_cast<int>(OtherSide(m_nextMoveSide))]);

	for (auto from : *this)
	{
		if (CanMove(from, kingLoc))
		{
			return true;
		}
	}
	return false;
}

SideType OtherSide(SideType side)
{
	return static_cast<SideType>((static_cast<byte>(side)+1) % 2);
}
