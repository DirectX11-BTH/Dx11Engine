#include "InputHandler.h"

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
	case WM_DESTROY: //Destroy window
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN: //If someone clicks left mouse button
		MessageBox(hWnd, L"CLICKED", L"TITLE", MB_OK); //hWnd is which app / window it belongs to
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}
