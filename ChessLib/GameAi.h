#pragma once

#include "boardstate.h"

class GameAi
{
public:
	GameAi();
	~GameAi();

	ChessMove DecideMove(const BoardState& board);

	int GameAi::GetBoardScore(const BoardState& board);
};

