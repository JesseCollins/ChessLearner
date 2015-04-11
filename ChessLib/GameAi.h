#pragma once

#include "boardstate.h"

class GameAi
{
public:
	GameAi();
	~GameAi();

	ChessMove DecideMove(const BoardState& board);

};

