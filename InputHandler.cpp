#include "InputHandler.h"
std::unique_ptr<DirectX::Mouse> InputHandler::mouse = nullptr;
std::unique_ptr<DirectX::Keyboard> InputHandler::keyboard = nullptr;
float2 InputHandler::lastMousePos;

LPDIRECTINPUT8 InputHandler::DirectInput;
DIMOUSESTATE InputHandler::lastMouseState;
IDirectInputDevice8* InputHandler::DIMouse = nullptr;
bool  InputHandler::trackMouse = true;

bool InputHandler::resetCursor = false;
using namespace DirectX;

InputHandler::InputHandler(HWND& primaryWindow)
{

	keyboard = std::make_unique<DirectX::Keyboard>();
	mouse = std::make_unique<DirectX::Mouse>();

	assert(primaryWindow != nullptr);
	assert(keyboard != nullptr);
	mouse->SetWindow(primaryWindow);

	HRESULT createInputSucc  = DirectInput8Create(DxHandler::hInstance,
        DIRECTINPUT_VERSION,
        IID_IDirectInput8,
        (void**)&DirectInput,
        NULL); 
	assert(SUCCEEDED(createInputSucc));

	HRESULT CreateDeviceSucc  = DirectInput->CreateDevice(GUID_SysMouse,
    &DIMouse,
    NULL);
	assert(SUCCEEDED(CreateDeviceSucc));

	HRESULT dataFormatMouseSucc = DIMouse->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(dataFormatMouseSucc));
	HRESULT setCoopSucc = DIMouse->SetCooperativeLevel(primaryWindow, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
	assert(SUCCEEDED(setCoopSucc));
}

InputHandler::InputHandler()
{
}

void InputHandler::handleInput()
{
	DIMOUSESTATE mouseState;
	DIMouse->Acquire();
	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState);

	if (trackMouse)
	{
		Camera::yaw += lastMouseState.lX * 0.002f;
		Camera::pitch -= lastMouseState.lY * 0.002f;
		lastMouseState = mouseState;
	}

}

LRESULT InputHandler::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	float degToRad = (XM_PI * 2.f) / 380.f;
	float deltaX = 0;
	float deltaY = 0;

	RECT windowRect;
	float windowPosX = 0.f;
	float windowPosY = 0.f;
	POINT currentMousePosition;
	POINT mouseResetPosition = { 0, 0 };

	switch (message)
	{

	case WM_ACTIVATEAPP:
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		DirectX::Mouse::ProcessMessage(message, wParam, lParam);
		break;
	case WM_CREATE:
			
		break;
	case WM_DESTROY: //Destroy window
		PostQuitMessage(0);
		break;
	case VK_ESCAPE:
		PostQuitMessage(0);
		break;
	case WM_LBUTTONUP: //If someone clicks left mouse button
		break;
	case WM_MOUSEHOVER:
		DirectX::Mouse::ProcessMessage(message, wParam, lParam);
		break;
	case WM_KEYDOWN: //When a key is pushed down.
		if (wParam == 0x57) //W Button, virtual keycodes.
		{
			Camera::zTranslation = -2.5f;
		}
		if (wParam == 0x53) //S Button
		{
			Camera::zTranslation = 2.5f;
		}

		if (wParam == 0x41) //A Button
		{
			Camera::xTranslation = 2.5f;
		}

		if (wParam == 0x44) //D Button
		{
			Camera::xTranslation = -2.5f;
		}

		if (wParam == 0x45) //E Button, up
		{
			Camera::yTranslation = 2.5f;
		}

		if (wParam == 0x51) //Q Button, down
		{
			Camera::yTranslation = -2.5f;
		}

		if (wParam == VK_SPACE)
			Camera::WALK_ON_TERRAIN = !Camera::WALK_ON_TERRAIN;

		if (wParam == VK_ESCAPE)
			exit(1);

		break;	
	case WM_KEYUP:
		if (wParam == 0x57) //W Button, virtual keycodes, see https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
		{
			Camera::zTranslation = 0.f;
		}

		if (wParam == 0x53) //S Button
		{
			Camera::zTranslation = 0.0f;
		}

		if (wParam == 0x41) //A Button
		{
			Camera::xTranslation = 0.0f;
		}

		if (wParam == 0x44) //D Button
		{
			Camera::xTranslation = 0.0f;
		}

		if (wParam == 0x45) //E Button, up
		{
			Camera::yTranslation = 0.0f;
		}

		if (wParam == 0x51) //Q Button, down
		{
			Camera::yTranslation = -0.0f;
		}

		break;
	case WM_SYSKEYUP:
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		break;
	case WM_KILLFOCUS:
		trackMouse = false;
		break;
	case WM_SETFOCUS:
		trackMouse = true;
		break;
	default:
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
