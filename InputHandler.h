#pragma once
#define WIN32_LEAN_AND_MEAN
#include <wrl.h>
#include <Windows.h>
#include "DXHandler.h"

class InputHandler
{
private:

public:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};