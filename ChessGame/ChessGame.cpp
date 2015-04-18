// ChessGame.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GameAi.h"


int _tmain(int argc, _TCHAR* argv[])
{
	BoardState b;
	GameAi ai;

	DWORD start = ::GetTickCount();

	//for (int i = 0; i < 1; ++i)
	{
		auto m = ai.DecideMoveImpl(b, 3, nullptr);
		//b.Move(m.From, m.To, nullptr);
	}
	DWORD total = ::GetTickCount() - start;

	wprintf(L"Time %f\n", total / 1000.);

	
	
	
	return 0;
}

