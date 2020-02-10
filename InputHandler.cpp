#include "InputHandler.h"
std::unique_ptr<DirectX::Mouse> InputHandler::mouse = nullptr;
std::unique_ptr<DirectX::Keyboard> InputHandler::keyboard = nullptr;
float2 InputHandler::lastMousePos;

using namespace DirectX;

InputHandler::InputHandler(HWND& primaryWindow)
{

	keyboard = std::make_unique<DirectX::Keyboard>();
	mouse = std::make_unique<DirectX::Mouse>();

	assert(primaryWindow != nullptr);
	assert(keyboard != nullptr);
	mouse->SetWindow(primaryWindow);
}

InputHandler::InputHandler()
{
}

void InputHandler::handleInput()
{
	auto keyboardState = DirectX::Keyboard::Get().GetState();
	if (keyboardState.W || keyboardState.Delete)
	{
		std::cout << "hello" << std::endl;
	}

	if (keyboardState.Back)
		std::cout << "Back" << std::endl;
	
	auto mouseState = mouse->GetState();

	if (mouseState.leftButton)
		mouse.get()->SetMode(DirectX::Mouse::MODE_RELATIVE);
	else
		mouse.get()->SetMode(DirectX::Mouse::MODE_ABSOLUTE);

	if (mouseState.leftButton)
		std::cout << "CLICKED WOOO" << std::endl;
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

		GetCursorPos(&currentMousePosition);

		if (currentMousePosition.x != mouseResetPosition.x && currentMousePosition.y != mouseResetPosition.y)
		{

			deltaX = LOWORD(lParam) - lastMousePos.x;
			deltaY = HIWORD(lParam) - lastMousePos.y;

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
			ClientToScreen(hWnd, &mouseResetPosition);

			std::cout << mouseResetPosition.x << " : " << mouseResetPosition.y << std::endl;
			std::cout << currentMousePosition.x << " = " << currentMousePosition.y << std::endl;

			//SetCursorPos(mouseResetPosition.y, mouseResetPosition.y);
		}
		else
			std::cout << "-";
		break;
	case WM_LBUTTONDOWN: //If someone clicks left mouse button
		break;
	case WM_MOUSEHOVER:
		DirectX::Mouse::ProcessMessage(message, wParam, lParam);
		break;
	case WM_KEYDOWN:
		//DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		if (wParam == 0x57) //W Button, virtual keycodes, see https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
		{
			Camera::zTranslation = -0.05f;
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
