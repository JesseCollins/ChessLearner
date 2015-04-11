#include "stdafx.h"
#include "GameAi.h"
#include <Windows.h>

GameAi::GameAi()
{
	::srand(::GetTickCount());
}


GameAi::~GameAi()
{
}


ChessMove GameAi::DecideMove(const BoardState& board)
{
	const auto& moves = board.ValidMoves();

	int choice = rand() % moves.size();

	return moves[choice];
}