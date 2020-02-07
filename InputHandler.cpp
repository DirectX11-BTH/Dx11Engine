#include "InputHandler.h"

InputHandler::InputHandler(HWND& primaryWindow)
{

	keyboard = std::make_unique<DirectX::Keyboard>();
	mouse = std::make_unique<DirectX::Mouse>();

	assert(primaryWindow != nullptr);
	//mouse->SetWindow(primaryWindow);
}

InputHandler::InputHandler()
{
}

void InputHandler::handleInput()
{
	auto keyboardState = keyboard->GetState();
	auto mouseState = mouse->GetState();

	if (keyboardState.W)
	{
		std::cout << "hello" << std::endl;
	}
}

LRESULT InputHandler::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//PAINTSTRUCT ps;
	//HDC hdc; //HDC is to handle		 context.

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
	case WM_DESTROY: //Destroy window
		PostQuitMessage(0);
		break;
	case WM_MOUSEMOVE:
		//ME MOUSE ME LYF
		//MessageBox(hWnd, L"MOVED", L"TITLE", MB_OK); //hWnd is which app / window it belongs to

	case WM_LBUTTONDOWN: //If someone clicks left mouse button
		//MessageBox(hWnd, L"CLICKED", L"TITLE", MB_OK); //hWnd is which app / window it belongs to
		break;
	case WM_MOUSEHOVER:
		DirectX::Mouse::ProcessMessage(message, wParam, lParam);
		break;
	case WM_KEYUP:
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
	case WM_SYSKEYUP:
		DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;

	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
