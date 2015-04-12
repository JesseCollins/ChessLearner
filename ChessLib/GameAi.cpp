#include "stdafx.h"
#include "GameAi.h"
#include <algorithm>
#include <Windows.h>
#include <random>

std::default_random_engine g_randomGenerator;

GameAi::GameAi()
	: m_bestMove(InvalidChessMove)
	, m_startTime(0)
	, m_finishedEvent(INVALID_HANDLE_VALUE)
{
	//g_randomGenerator.seed(::GetTickCount());
	g_randomGenerator.seed(0);
}


GameAi::~GameAi()
{
	::CloseHandle(m_finishedEvent);
}

int GameAi::GetBoardScore(const BoardState& board)
{
	int total = 0;

	const static int scores[] = { 0, 1000, 3000, 3000, 5000, 9000, 100000 };
	const static int multiplier[] = { 1, -1 };	

	if (board.IsCheckmate())
	{
		return 1000000000 * multiplier[static_cast<int>(OtherSide(board.NextSide()))];
	}

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

	m_finishedEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);

	auto newThread = ::CreateThread(nullptr, 0, &GameAi::WorkerThreadStatic, this, 0, &threadId);
	::CloseHandle(newThread);
}

DWORD WINAPI GameAi::WorkerThread()
{
	auto move = DecideMoveImpl(*m_board, 2, nullptr);
	m_bestMove = move;
	m_elapsedTime = ::GetTickCount() - m_startTime;
	SetEvent(m_finishedEvent);
	return 0;
}

ChessMove GameAi::DecideMoveImpl(const BoardState& board, int depth, int* scoreAfterMove)
{
	const auto& moves = board.ValidMoves();
	if (moves.size() == 0)
	{
		return InvalidChessMove;
	}

	std::vector<int> scores;

	int max = -2000000000;
	int min = 2000000000;

	for (auto m : moves)
	{
		auto temp = board;
		temp.Move(m.From, m.To);
		int score = 0;

		if (depth == 0)
		{
			score = GetBoardScore(temp);
		}
		else
		{
			if (!DecideMoveImpl(temp, depth - 1, &score).IsValid())
			{
				score = GetBoardScore(temp);
			}
		}

		if (score > max) max = score;
		if (score < min) min = score;

		scores.push_back(score);
	}

	int best = (board.NextSide() == SideType::White ? max : min);

	int count = std::count(scores.begin(), scores.end(), best);

	std::uniform_int_distribution<int> distribution(0, count-1);
	int choice = distribution(g_randomGenerator);

	for (unsigned i = 0; i < scores.size(); ++i)
	{
		if (scores[i] == best)
		{
			if (choice == 0)
			{
				if (scoreAfterMove) *scoreAfterMove = best;
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