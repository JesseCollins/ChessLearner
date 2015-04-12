#pragma once

#include "boardstate.h"
#include <windows.h>

class GameAi
{
public:
	GameAi();
	~GameAi();

	void StartDecideMove(const BoardState& board);

	int GameAi::GetBoardScore(const BoardState& board);

	bool IsFinished() const
	{
		return m_bestMove.IsValid();
	}

	ChessMove GetMove() const
	{
		return m_bestMove;
	}

	DWORD GetElapsedTime() const
	{
		return m_elapsedTime;
	}

private:
	DWORD m_startTime;
	DWORD m_elapsedTime;
	ChessMove m_bestMove;
	const BoardState* m_board;

	ChessMove DecideMoveImpl(const BoardState& board, int depth, int* scoreAfterMove);
	static DWORD WINAPI WorkerThreadStatic(_In_  LPVOID lpParameter);
	DWORD WINAPI WorkerThread();
};

