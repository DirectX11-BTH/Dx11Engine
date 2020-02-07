#pragma once
#define WIN32_LEAN_AND_MEAN
#include <wrl.h>
#include <Windows.h>
#include "DXHandler.h"
#include <Mouse.h>
#include <Keyboard.h>
#include <Windows.h>

class InputHandler
{
private:

	DirectX::XMVECTOR lastMousePos;

public:
	std::unique_ptr<DirectX::Mouse> mouse;
	std::unique_ptr<DirectX::Keyboard> keyboard;

	InputHandler(HWND& primaryWindow);
	InputHandler();

	void handleInput();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};