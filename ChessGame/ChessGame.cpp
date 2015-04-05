// ChessGame.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"



int _tmain(int argc, _TCHAR* argv[])
{
	BoardState b;
	
	b.MovePgn("e4");
	b.MovePgn("e5");
	b.MovePgn("Nf3");
	b.MovePgn("Nc6");
	b.MovePgn("Bb5");
	b.MovePgn("a6");
	b.MovePgn("Ba4");
	b.MovePgn("Nf6");
	b.MovePgn("O-O");
	b.MovePgn("Be7");
	b.MovePgn("Re1");
	b.MovePgn("b5");
	b.MovePgn("Bb3");
	b.MovePgn("d6");
	b.MovePgn("c3");
	b.MovePgn("O-O");
	b.MovePgn("h3");
	b.MovePgn("Nb8");
	b.MovePgn("d4");
	b.MovePgn("Nbd7");
	b.MovePgn("c4");
	b.MovePgn("c6");
	b.MovePgn("cxb5");
	b.MovePgn("axb5");
	b.MovePgn("Nc3");
	b.MovePgn("Bb7");
	b.MovePgn("Bg5");
	b.MovePgn("b4");
	b.MovePgn("Nb1");
	b.MovePgn("h6");
	b.MovePgn("Bh4");
	b.MovePgn("c5");
	b.MovePgn("dxe5");
	b.MovePgn("Nxe4");
	b.MovePgn("Bxe7");
	b.MovePgn("Qxe7");
	b.MovePgn("exd6");
	b.MovePgn("Qf6");
	b.MovePgn("Nbd2");
	b.MovePgn("Nxd6");
	b.MovePgn("Nc4");
	b.MovePgn("Nxc4");
	b.MovePgn("Bxc4");
	b.MovePgn("Nb6");
	b.MovePgn("Ne5");
	b.MovePgn("Rae8");
	b.MovePgn("Bxf7+");
	b.MovePgn("Rxf7");
	b.MovePgn("Nxf7");
	b.MovePgn("Rxe1+");
	b.MovePgn("Qxe1");
	b.MovePgn("Kxf7");
	b.MovePgn("Qe3");
	b.MovePgn("Qg5");
	b.MovePgn("Qxg5");
	b.MovePgn("hxg5");
	b.MovePgn("b3");
	b.MovePgn("Ke6");
	b.MovePgn("a3");
	b.MovePgn("Kd6");
	b.MovePgn("axb4");
	b.MovePgn("cxb4");
	b.MovePgn("Ra5");
	b.MovePgn("Nd5");
	b.MovePgn("f3");
	b.MovePgn("Bc8");
	b.MovePgn("Kf2");
	b.MovePgn("Bf5");
	b.MovePgn("Ra7");
	b.MovePgn("g6");
	b.MovePgn("Ra6+");
	b.MovePgn("Kc5");
	b.MovePgn("Ke1");
	b.MovePgn("Nf4");
	b.MovePgn("g3");
	b.MovePgn("Nxh3");
	b.MovePgn("Kd2");
	b.MovePgn("Kb5");
	b.MovePgn("Rd6");
	b.MovePgn("Kc5");
	b.MovePgn("Ra6");
	b.MovePgn("Nf2");
	b.MovePgn("g4");
	b.MovePgn("Bd3");
	b.MovePgn("Re6");

	b.Print();

	
	
	return 0;
}

