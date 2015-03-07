
#include <assert.h>

enum class PieceType : unsigned char
{
	Empty,
	Pawn,
	Bishop,
	Knight,
	Rook,
	Queen,
	King
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

enum class SideType : unsigned char
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

	PieceType		Type : 7;
	SideType		Side : 1;
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

	unsigned char Raw() const
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

private:
	void Init(int x, int y)
	{
		auto val = x + y * 8;
		assert(val >= 0 && val < 64);
		m_sq = static_cast<unsigned char>(val);
	}

	unsigned char m_sq;
};

class BoardState
{
public:

	BoardState()
		: m_nextModeSide(SideType::White)
	{
		static_assert(static_cast<int>(PieceType::King) < (1 << 3), "Ensure PieceType can fit in 3 bits");

		memset(m_board, sizeof(m_board), 0);

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
		return m_board[col + row * 8];
	}

	Piece Get(BoardLocation loc) const
	{
		return m_board[loc.Raw()];
	}

	/*
	bool Move(const char* from, const char* to)
	{
		return Move(
			from[0] - 'a', 
			7 - (from[1] - '1'),
			to[0] - 'a',
			7 - (to[1] - '1'));
	}*/

	bool CanMove(BoardLocation from, BoardLocation to) const
	{
		auto fromPiece = Get(from);
		auto toPiece = Get(to);

		if (fromPiece.Type == PieceType::Empty) return false;
		if (fromPiece.Side != m_nextModeSide) return false;

		if (toPiece.Type != PieceType::Empty && toPiece.Side == fromPiece.Side) return false;

		if (fromPiece.Type == PieceType::Pawn)
		{
			
			if (m_nextModeSide == SideType::White)
			{
				if (from.Y() >= to.Y()) return false;

				if (from.Y() - to.Y() > 2) return false;
			}
		}

		return true;
	}

	bool Move(BoardLocation from, BoardLocation to)
	{
		if (!CanMove(from, to))
		{
			return false;
		}

		Set(to, Get(from));
		Set(from, Piece(PieceType::Empty, SideType::White));

		return true;
	}

protected:

	void Set(BoardLocation loc, Piece p)
	{
		m_board[loc.Raw()] = p;
	}

	// TODO: we should be able to cut this by half, since we can represent a board square
	// in 4 bits (nibble)
	Piece m_board[64];
	bool m_whiteCanCastle;
	bool m_blackCanCastle;
	SideType m_nextModeSide;
};
