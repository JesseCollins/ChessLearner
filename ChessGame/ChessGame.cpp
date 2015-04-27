// ChessGame.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GameAi.h"


int _tmain(int argc, _TCHAR* argv[])
{

    // Right now just using this to get insight into perf
	BoardState b;
	GameAi ai;

	DWORD start = ::GetTickCount();

	ai.DecideMoveImpl(b, 3, nullptr);

	DWORD total = ::GetTickCount() - start;
	wprintf(L"scores:%d canmove:%d\n", g_boardScoreCalls, g_canMoveCalls);
	wprintf(L"Time %f\n", total / 1000.);

	
	
	
	return 0;
}

