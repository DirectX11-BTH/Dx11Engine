#pragma once
#define WIN32_LEAN_AND_MEAN
#include <wrl.h>
#include <Windows.h>
#include <string>
#include <tchar.h>
#include <atlstr.h>
#include <iostream>
#include "InputHandler.h"
#include "DXHandler.h"

class RenderWindow
{
private:
	HWND hWnd;
	WNDCLASSEX window;
	int nCmdShow = 5; //"Activates the window and displays it in its current size and position."
public:
	HWND& createWindow(int width, int height, std::string title, std::string applicationName);
};