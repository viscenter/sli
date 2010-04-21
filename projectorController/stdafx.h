// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
#pragma once
#include <winsock2.h>
#include <objbase.h>

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <stdio.h>
#include <tchar.h>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "OLE32.lib") 

#include <cv.h>
#include <highgui.h>
#include <math.h>
#include <direct.h>
#include <conio.h>


