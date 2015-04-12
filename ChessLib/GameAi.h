#pragma once

#include "boardstate.h"
#include <windows.h>

class GameAi
{
public:
	GameAi();
	~GameAi();

	void StartDecideMove(const BoardState& board);

	int GetBoardScore(const BoardState& board);

	bool IsFinished() const
	{
		return ::WaitForSingleObjectEx(m_finishedEvent, 0, TRUE) == WAIT_OBJECT_0;
	}

	void WaitUntilFinished() const
	{
		::WaitForSingleObjectEx(m_finishedEvent, INFINITE, TRUE);
	}

	ChessMove GetMove() const
	{
		return m_bestMove;
	}

	DWORD GetElapsedTime() const
	{
		return m_elapsedTime;
	}

	ChessMove DecideMoveImpl(const BoardState& board, int depth, int* scoreAfterMove);

private:
	DWORD m_startTime;
	DWORD m_elapsedTime;
	ChessMove m_bestMove;
	const BoardState* m_board;

	HANDLE m_finishedEvent;

	
	static DWORD WINAPI WorkerThreadStatic(_In_  LPVOID lpParameter);
	DWORD WINAPI WorkerThread();
};

