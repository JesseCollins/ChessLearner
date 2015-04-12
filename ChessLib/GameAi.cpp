#include "stdafx.h"
#include "GameAi.h"
#include <algorithm>
#include <Windows.h>

GameAi::GameAi()
	: m_bestMove(InvalidChessMove)
	, m_startTime(0)
{
	::srand(::GetTickCount());
}


GameAi::~GameAi()
{
}

int GameAi::GetBoardScore(const BoardState& board)
{
	int total = 0;

	const static int scores[] = { 0, 1, 3, 3, 5, 9, 100 };
	const static int multiplier[] = { 1, -1 };	
	for (auto it : board)
	{
		auto p = board.Get(it);		
		const int score = scores[static_cast<int>(p.Type)] * multiplier[static_cast<int>(p.Side)];
		total += score;
	}


	return total;
}


void GameAi::StartDecideMove(const BoardState& board)
{
	DWORD threadId;

	m_startTime = ::GetTickCount();

	m_board = &board;

	auto newThread = ::CreateThread(nullptr, 0, &GameAi::WorkerThreadStatic, this, 0, &threadId);
	::CloseHandle(newThread);
}

DWORD WINAPI GameAi::WorkerThread()
{
	auto move = DecideMoveImpl(*m_board);
	m_bestMove = move;
	m_elapsedTime = ::GetTickCount() - m_startTime;
	return 0;
}

ChessMove GameAi::DecideMoveImpl(const BoardState& board)
{
	const auto& moves = board.ValidMoves();
	std::vector<int> scores;

	int max = -2000000000;
	int min = 2000000000;

	for (auto m : moves)
	{
		auto temp = board;
		temp.Move(m.From, m.To);
		const int score = GetBoardScore(temp);
		if (score > max) max = score;
		if (score < min) min = score;

		scores.push_back(score);
	}

	int best = (board.NextSide() == SideType::White ? max : min);

	int count = std::count(scores.begin(), scores.end(), best);

	int choice = rand() % count;

	for (unsigned i = 0; i < scores.size(); ++i)
	{
		if (scores[i] == best)
		{
			if (choice == 0)
			{
				return moves[i];
			}
			choice--;
		}
	}

	throw "We shouldn't get here";
	return moves[0];
}

DWORD WINAPI GameAi::WorkerThreadStatic(_In_  LPVOID lpParameter)
{
	auto pThis = reinterpret_cast<GameAi*>(lpParameter);
	return pThis->WorkerThread();

}