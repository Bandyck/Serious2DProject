#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <queue>
#include <functional>

inline bool operator==(const POINT & p1, const POINT & p2)
{
	if (p1.x == p2.x && p1.y == p2.y)
		return true;
	return false;
}