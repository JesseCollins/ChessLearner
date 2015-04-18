#pragma once

#include <assert.h>
#include <functional>
#include <vector>
#include <bitset>

extern int g_canMoveCalls;
extern int g_boardScoreCalls;

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
	BoardLocation() : m_sq(64) {}

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

	BoardLocation operator=(const BoardLocation& other)
	{
		m_sq = other.m_sq;
		return *this;
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

	std::string ToString() const
	{
		return "xx";
	}

private:
	void Init(int x, int y)
	{
		auto val = x + y * 8;
		assert(val >= 0 && val <= 64); // 64 is valid for end()
		m_sq = static_cast<byte>(val);
	}


	enum class alphaNum : byte {
		a8, b8, c8, d8, e8, f8, g8, h8,
		a7, b7, c7, d7, e7, f7, g7, h7,
		a6, b6, c6, d6, e6, f6, g6, h6,
		a5, b5, c5, d5, e5, f5, g5, h5,
		a4, b4, c4, d4, e4, f4, g4, h4,
		a3, b3, c3, d3, e3, f3, g3, h3,
		a2, b2, c2, d2, e2, f2, g2, h2,
		a1, b1, c1, d1, e1, f1, g1, h1
	};

	union
	{
		byte m_sq;
		alphaNum dummy;
	};
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

		InitializeKingPositions();
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

	inline bool CanMove(BoardLocation from, BoardLocation to) const;
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

	bool MovePgn(const char* pgn);

	bool CanTakeKing() const;
	
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
		if (!IsCheck()) return false;

		for (auto m : ValidMoves())
		{
			auto next = *this;
			next.Move(m.From, m.To, true);
			if (!next.CanTakeKing()) return false;
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

	bool MoveImpl(BoardLocation from, BoardLocation to, MoveCallback callback = nullptr);

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

	void InitializeKingPositions();

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
	byte m_enPassantCol;
	BoardLocation m_kingPosition[2];
};


class BoardController
{};

