#include "InputHandler.h"
std::unique_ptr<DirectX::Mouse> InputHandler::mouse = nullptr;
std::unique_ptr<DirectX::Keyboard> InputHandler::keyboard = nullptr;
float2 InputHandler::lastMousePos;

LPDIRECTINPUT8 InputHandler::DirectInput;
DIMOUSESTATE InputHandler::lastMouseState;
IDirectInputDevice8* InputHandler::DIMouse = nullptr;

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

	Camera::yaw += lastMouseState.lX * 0.002f;
	Camera::pitch -= lastMouseState.lY * 0.002f;

	lastMouseState = mouseState;
}

LRESULT InputHandler::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//PAINTSTRUCT ps;
	//HDC hdc; //HDC is to handle		 context.

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
	/*case WM_PAINT: //If the window is resized
		hdc = BeginPaint(hWnd, &ps); //Indicate it's using paint func

		EndPaint(hWnd, &ps); //Return control of the paint func
		break;*/

	case WM_ACTIVATEAPP:
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		DirectX::Mouse::ProcessMessage(message, wParam, lParam);
		break;
	case WM_CREATE:
			
		break;
	case WM_DESTROY: //Destroy window
		PostQuitMessage(0);
		break;
	case WM_MOUSEMOVE:
		//float x = LOWORD(lParam);
		//float y = HIWORD(lParam);

		//GetCursorPos(&currentMousePosition);

		//if (currentMousePosition.x != mouseResetPosition.x && currentMousePosition.y != mouseResetPosition.y && !resetCursor)
		//if(leftMouseButtonDown)
		/*if (LOWORD(lParam) != 600 / 2 && HIWORD(lParam) != 500/2)
		{

			deltaX = LOWORD(lParam) - lastMousePos.x; //LOWORD(lParam) = x in indow
			deltaY = HIWORD(lParam) - lastMousePos.y; //HIWORD(lParam) = y in window

			Camera::pitch += -30 * deltaY / 600 * degToRad; //Up and down
			Camera::yaw += -30 * -deltaX / 500 * degToRad; //Side to side
			
			GetWindowRect(hWnd, &windowRect);
			windowPosX = windowRect.left;
			windowPosY = windowRect.top;

			XMFLOAT4 camPos;
			XMStoreFloat4(&camPos, (Camera::cameraTarget + Camera::cameraPosition));
			lastMousePos = { (float)LOWORD(lParam), (float)HIWORD(lParam) };

			mouseResetPosition.x = 600 / 2;
			mouseResetPosition.y = 500 / 2;
			ClientToScreen(hWnd, &mouseResetPosition); //From window space to your entire screen
			std::cout << mouseResetPosition.x << " : " << mouseResetPosition.y << std::endl;

			SetCursorPos(mouseResetPosition.y, mouseResetPosition.y);
			std::cout << currentMousePosition.x << " = " << currentMousePosition.y << std::endl;
			resetCursor = true;
		}
		else
			resetCursor = false;
		break;*/
	case WM_LBUTTONDOWN: //If someone clicks left mouse button
		//leftMouseButtonDown = true;
		break;
	case WM_LBUTTONUP: //If someone clicks left mouse button
		//leftMouseButtonDown = false;
		break;
	case WM_MOUSEHOVER:
		DirectX::Mouse::ProcessMessage(message, wParam, lParam);
		break;
	case WM_KEYDOWN:
		//DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		if (wParam == 0x57) //W Button, virtual keycodes, see https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
		{
			Camera::zTranslation = -0.7f;
		}
		if (wParam == 0x53) //S Button
		{
			Camera::zTranslation = 0.05f;
		}

		if (wParam == 0x41) //A Button
		{
			Camera::xTranslation = 0.05f;
		}

		if (wParam == 0x44) //D Button
		{
			Camera::xTranslation = -0.05f;
		}

		if (wParam == 0x45) //E Button, up
		{
			Camera::yTranslation = 0.05f;
		}

		if (wParam == 0x51) //Q Button, down
		{
			Camera::yTranslation = -0.05f;
		}

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
	default:
		//return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
