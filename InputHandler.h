#pragma once
#define WIN32_LEAN_AND_MEAN
#include <wrl.h>
#include <Windows.h>
#include "DXHandler.h"
#include <Mouse.h>
#include <Keyboard.h>

struct float2
{
	float x, y = 0;
};

class InputHandler
{
private:

	static float2 lastMousePos;

public:
	static std::unique_ptr<DirectX::Mouse> mouse;
	static std::unique_ptr<DirectX::Keyboard> keyboard;

	InputHandler(HWND& primaryWindow);
	InputHandler();

	void handleInput();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};