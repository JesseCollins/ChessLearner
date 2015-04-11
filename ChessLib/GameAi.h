#pragma once

#include "boardstate.h"
#include <windows.h>

class GameAi
{
public:
	GameAi();
	~GameAi();

	ChessMove DecideMove(const BoardState& board);

	int GameAi::GetBoardScore(const BoardState& board);

private:
	ChessMove m_bestMove;
	const BoardState* m_board;

	ChessMove DecideMoveImpl(const BoardState& board);
	static DWORD WINAPI WorkerThreadStatic(_In_  LPVOID lpParameter);
	DWORD WINAPI WorkerThread();
};

