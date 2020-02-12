#pragma once

#define DIRECTINPUT_VERSION 0x0800
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include <dinput.h>
#include <InitGuid.h>

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
	static bool justSetMouse;
	static float2 lastMousePos;
	static DIMOUSESTATE lastMouseState;
	static IDirectInputDevice8* DIMouse;

	static LPDIRECTINPUT8 DirectInput;

public:
	static std::unique_ptr<DirectX::Mouse> mouse;
	static std::unique_ptr<DirectX::Keyboard> keyboard;
	static bool trackMouse;

	static bool resetCursor;

	InputHandler(HWND& primaryWindow);
	InputHandler();

	void handleInput();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};