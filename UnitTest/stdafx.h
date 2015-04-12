// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

// Headers for CppUnitTest
#include "CppUnitTest.h"

#include <cstring>
#include <iostream>

#include "BoardState.h"

std::basic_string<wchar_t> ToString(SideType s);

std::basic_string<wchar_t> ToString(PieceType p);

std::basic_string<wchar_t> ToString(const Piece& p);

std::basic_string<wchar_t> ToString(const BoardLocation& loc);

// TODO: reference additional headers your program requires here
