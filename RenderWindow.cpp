#include "RenderWindow.h"
#include "Engine.h"
#define D3D_COMPILE_STANDARD_FILE_INCLUDE ((ID3DInclude*)(UINT_PTR)1);
namespace wrl = Microsoft::WRL;
HWND& RenderWindow::createWindow(int width, int height, std::string title, std::string applicationName)
{
	TCHAR szWindowClass[20];
	TCHAR szWindowTitle[20];
	_tcscpy_s(szWindowClass, CA2T(applicationName.c_str()));
	_tcscpy_s(szWindowTitle, CA2T(title.c_str()));
	FILE* fp;
	AllocConsole();
	freopen_s(&fp, "CONOUT$", "w", stdout);
	window.cbSize = sizeof(WNDCLASSEX);
	window.style = CS_HREDRAW | CS_VREDRAW;
	window.lpfnWndProc = InputHandler::WndProc; //The function we want to call when we get an event. Could technically call function WndProc anything.
	window.cbClsExtra = 0;
	window.cbWndExtra = 0;
	window.hInstance = hInstance;
	window.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	window.hCursor = LoadCursor(NULL, IDC_ARROW);
	window.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	window.lpszMenuName = NULL;
	window.lpszClassName = szWindowClass;
	window.hIconSm = LoadIcon(window.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&(this->window)))
	{
		MessageBox(NULL,
			_T("Call to registerclassex failed!"),
			_T(""),
			NULL);
	};

	this->hWnd = CreateWindow(
		szWindowClass,
		szWindowTitle,
		WS_OVERLAPPEDWINDOW, //Type of window
		CW_USEDEFAULT, CW_USEDEFAULT, //Startpos
		width, height, //Size
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to create window failed!"),
			_T("Windows Desktop Guided Tour!"),
			NULL);
	}

	ShowWindow(hWnd, nCmdShow); //Actually displays the window
	UpdateWindow(hWnd);

	return this->hWnd;
}
