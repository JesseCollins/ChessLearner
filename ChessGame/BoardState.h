
#include <assert.h>

#pragma once

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

int GetHomeRow(SideType side);

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

	bool operator==(const BoardLocation& other)
	{
		return m_sq == other.m_sq;
	}

	bool operator!=(const BoardLocation& other)
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

class BoardState
{
public:

	BoardState()
		: m_nextMoveSide(SideType::White)
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

	bool CanMove(BoardLocation from, BoardLocation to) const
	{
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

				// Must capture
				if (toPiece.Type == PieceType::Empty) return false;
			}
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
			if (ydist == 0 && from.X() == 4 && (to.X() == 2 || to.X() == 6))
			{
				// TODO: handle all the fancy cases.
				return true;
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
		return true;
	}

	bool Move(BoardLocation from, BoardLocation to)
	{
		if (!CanMove(from, to))
		{
			return false;
		}
		
		if (Get(from).Type == PieceType::King)
		{
			if (from.X() + 2 == to.X())
			{
				MovePiece(BoardLocation(7, from.Y()), BoardLocation(5, from.Y()));
			}
			if (from.X() - 2 == to.X())
			{
				MovePiece(BoardLocation(0, from.Y()), BoardLocation(2, from.Y()));
			}
		}

		MovePiece(from, to);

		this->m_nextMoveSide = m_nextMoveSide == SideType::White ? SideType::Black : SideType::White;

		return true;
	}

	bool MovePgn(const char* pgn);

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

protected:

	void MovePiece(BoardLocation from, BoardLocation to)
	{
		Set(to, Get(from));
		Set(from, Piece(PieceType::Empty, SideType::White));
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

	bool m_whiteCanCastle : 1;
	bool m_blackCanCastle : 1;
	SideType m_nextMoveSide : 1;
};


