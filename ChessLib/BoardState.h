#pragma once

#include <assert.h>
#include <functional>
#include <vector>
#include <bitset>

typedef unsigned char byte;

enum class PieceType : byte
{
	Empty,
	Pawn,
	Bishop,
	Knight,
	Rook,
	Queen,
	King,
	Invalid
};

const char PieceTypeChars[] =
{
	'.',
	'P',
	'B',
	'N',
	'R',
	'Q',
	'K'
};

enum class SideType : byte
{
	White,
	Black
};


struct Piece
{
	Piece()
		: Type(PieceType::Empty)
		, Side(SideType::White) {}

	Piece(PieceType p, SideType s)
		: Type(p)
		, Side(s) {}

	Piece(byte b)
	{
		RawValue = b;
		assert(Type < PieceType::Invalid);
	}

	bool operator==(const Piece& other) const
	{
		return (
			other.Side == this->Side
			&&	other.Type == this->Type
			);
	}

	bool operator!=(const Piece& other) const
	{
		return !(*this == other);
	}

	char GetChar() const
	{
		return PieceTypeChars[static_cast<unsigned char>(Type)];
	}

	char GetCharWithSide() const
	{
		if (Side == SideType::White)
			return GetChar();
		else
			return static_cast<char>(::tolower(GetChar()));
	}

	SideType GetSide() const
	{
		return Side;
	}

	union
	{
		struct
		{
			PieceType		Type : 3;
			SideType		Side : 1;
		};
		byte RawValue;
	};

};

class BoardLocation
{
public:
	BoardLocation(int x, int y)
	{
		Init(x, y);
	}

	BoardLocation(const char* str)
	{
		assert(str[0] >= 'a' && str[0] <= 'h');
		assert(str[1] >= '1' && str[1] <= '8');
		Init(str[0] - 'a', 7 - (str[1] - '1'));
	}

	BoardLocation(const BoardLocation& other)
	{
		m_sq = other.m_sq;
	}

	BoardLocation(byte raw)
	{
		m_sq = raw;
	}

	byte Raw() const
	{
		return m_sq;
	}

	int X() const
	{
		return m_sq % 8;
	}

	int Y() const
	{
		return m_sq / 8;
	}

	bool operator==(const BoardLocation& other) const
	{
		return m_sq == other.m_sq;
	}

	bool operator!=(const BoardLocation& other) const
	{
		return !(*this == other);
	}

private:
	void Init(int x, int y)
	{
		auto val = x + y * 8;
		assert(val >= 0 && val <= 64); // 64 is valid for end()
		m_sq = static_cast<byte>(val);
	}

	byte m_sq;
};
extern const BoardLocation InvalidBoardLocation;

PieceType GetPieceType(const char c);
Piece GetPieceAndSide(const char c);
int GetHomeRow(SideType side);
int GetEnPassantRow(SideType side);
SideType OtherSide(SideType side);


// Encapsulate one chess move
class ChessMove
{
public:
	bool IsValid() const { return From != InvalidBoardLocation; }

	// A move is represented with a "from" and "to" location,
	// castling is represented by moving the king 2 squares either
	// left or right.
	BoardLocation From;
	BoardLocation To;
	PieceType PromotionPiece; // not always needed!
};

extern const ChessMove InvalidChessMove;

class BoardState
{
public:

	BoardState()
		: m_nextMoveSide(SideType::White)
		, m_enPassantCol(-1)
	{
		static_assert(static_cast<int>(PieceType::King) < (1 << 3), "Ensure PieceType can fit in 3 bits");

		memset(m_board, 0, sizeof(m_board));

		for (int i = 0; i < 8; ++i)
		{
			Set({ i, 1 }, Piece{ PieceType::Pawn, SideType::Black });
			Set({ i, 6 }, Piece{ PieceType::Pawn, SideType::White });
		}

		for (int i = 0; i < 2; ++i)
		{
			SideType side = static_cast<SideType>(i);
			int row = (side == SideType::Black) ? 0 : 7;

			Set({ 0, row }, Piece{ PieceType::Rook, side });
			Set({ 1, row }, Piece{ PieceType::Knight, side });
			Set({ 2, row }, Piece{ PieceType::Bishop, side });
			Set({ 3, row }, Piece{ PieceType::Queen, side });
			Set({ 4, row }, Piece{ PieceType::King, side });
			Set({ 5, row }, Piece{ PieceType::Bishop, side });
			Set({ 6, row }, Piece{ PieceType::Knight, side });
			Set({ 7, row }, Piece{ PieceType::Rook, side });
		}
	}

	BoardState(const char* board, SideType nextMove);	

#ifdef ENABLE_PRINT
	void Print() const
	{
		for (int r = 0; r < 8; ++r)
		{
			for (int c = 0; c < 8; ++c)
			{
				std::cout << Get(c, r).GetCharWithSide() << ' ';
			}
			std::cout << std::endl;
		}

	}
#endif

	Piece Get(int col, int row) const
	{
		return Get(col + row * 8);
	}

	Piece Get(BoardLocation loc) const
	{
		return Get(loc.Raw());
	}

	Piece Get(byte rawLocation) const
	{
		const auto adjustment = (rawLocation % 2) * 4;
		const byte value = (m_board[rawLocation / 2] >> adjustment) & 0xf;
		return Piece(value);
	}

	static int Sign(int x)
	{
		return (0 < x) - (x < 0);
	}

	bool IsObstructed(BoardLocation from, BoardLocation to) const
	{
		int dx = Sign(to.X() - from.X());
		int dy = Sign(to.Y() - from.Y());

		for (
			auto it = BoardLocation(from.X() + dx, from.Y() + dy);
			it != to;
			it = BoardLocation(it.X() + dx, it.Y() + dy))
		{
			if (Get(it).Type != PieceType::Empty)
			{
				return true;
			}
		}
		return false;
	}

	bool CanMove(BoardLocation from, BoardLocation to) const;
	bool CanCastle(BoardLocation from, BoardLocation to) const;

	typedef std::function<void(BoardLocation, BoardLocation)> MoveCallback;

	bool Move(BoardLocation from, BoardLocation to, bool force=false)
	{
		if (!force && !CanMove(from, to))
		{
			return false;
		}		
		return MoveImpl(from, to);
	}

	bool Move(BoardLocation from, BoardLocation to, MoveCallback callback)
	{
		if (!CanMove(from, to)) return false;
		return MoveImpl(from, to, callback);
	}

	BoardState Move2(BoardLocation from, BoardLocation to, bool force=false) const
	{
		BoardState newState(*this);
		newState.Move(from, to, force);
		return newState;
	}

	bool MovePgn(const char* pgn);

	bool CanTakeKing() const
	{
		for (auto from : *this)
		{
			for (auto to : *this)
			{
				if (Get(to).Type == PieceType::King && CanMove(from, to))
				{
					return true;
				}
			}
		}
		return false;
	}

	bool IsCheck() const
	{
		// This is check if the opponent could take the king if she had another free move
		auto temp = *this;
		temp.m_nextMoveSide = OtherSide(temp.m_nextMoveSide);
		if (temp.CanTakeKing())
		{
			return true;
		}
		return false;
	}

	bool IsCheckmate() const
	{
		// Checkmate: for every possible move, next move can take king!
		for (auto from : *this)
		{
			for (auto to : *this)
			{
				if (CanMove(from, to))
				{
					auto newBoard = Move2(from, to);
					if (!newBoard.CanTakeKing()) return false;
				}
			}
		}
		return true;
	}

	class Iterator
	{
	public:
		Iterator(BoardLocation loc)
			: m_location(loc)
		{ }

		Iterator& operator=(const Iterator& other)
		{
			m_location = other.m_location;
			return *this;
		}

		Iterator operator+(int amount) const
		{
			return Iterator(BoardLocation(m_location.Raw()+amount));
		}

		Iterator operator++()
		{
			m_location = BoardLocation(m_location.Raw() + 1);
			return (*this);
		}

		Iterator operator++(int dummy)
		{
			m_location = BoardLocation(m_location.Raw() + 1);
			return (*this);
		}

		BoardLocation operator*()
		{
			return m_location;
		}

		bool operator==(const Iterator& other) const
		{
			return this->m_location.Raw() == other.m_location.Raw();
		}

		bool operator!=(const Iterator& other) const
		{
			return !(*this == other);
		}
		
		BoardLocation m_location;
	};

	Iterator begin() const
	{
		return Iterator(BoardLocation(0, 0));
	}

	Iterator end() const
	{
		return Iterator(BoardLocation(0, 8));
	}

	typedef std::vector<ChessMove> MoveCollection;
	MoveCollection ValidMoves() const;

	SideType NextSide() const
	{
		return m_nextMoveSide;
	}

protected:

	bool MoveImpl(BoardLocation from, BoardLocation to, MoveCallback callback=nullptr)
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

	void MovePiece(BoardLocation from, BoardLocation to, MoveCallback callback)
	{
		if (callback)
		{
			if (Get(to).Type != PieceType::Empty)
			{
				callback(to, InvalidBoardLocation);
			}			
		}

		if (to != InvalidBoardLocation)	Set(to, Get(from));
		if (from != InvalidBoardLocation) Set(from, Piece(PieceType::Empty, SideType::White));

		if (callback)
		{
			callback(from, to);
		}
	}

	void Set(BoardLocation loc, Piece p)
	{
		//m_board[loc.Raw()] = p;

		const auto location = loc.Raw();
		const auto adjustment = (location % 2) * 4;

		const byte mask = 0xf << adjustment;
		const byte maskedValue = (p.RawValue << adjustment) & mask;

		m_board[location / 2] = maskedValue | (m_board[location / 2] & ~mask);

		assert(Get(loc) == p);
	}

	// We can fit the board into 32 bytes, each square takes a nibble
	unsigned char m_board[32];

	
	std::bitset<6> m_hasPieceMoved;
	// 0 - white king
	// 1 - white rook 1
	// 2 - white rook 8
	// 3 - black king
	// 4 - black rook 1
	// 5 - black rook 8
	

	SideType m_nextMoveSide : 1;
	int m_enPassantCol;
};


class BoardController
{};

