#pragma once

#define WIN32_LEAN_AND_MEAN
#include <d3dcompiler.h>
#include <dxgi.h>
#include <d3d11.h>
#include <assert.h>
#include <iostream>
#include <Windows.h>
//#include <d3dcompiler.h>
#include <tchar.h>
#include <string>
#include <wrl.h>
#include <vector>
#include <stdint.h>
#include <chrono>

#include <DirectXHelpers.h>
#include <SimpleMath.h>
#include <WICTextureLoader.h>
#include <math.h>

#pragma comment(lib, "gdi32")
#pragma comment(lib, "d3d11") 
#pragma comment(lib, "d3dcompiler.lib")


class DxHandler
{
private:

public:
	void initalizeDeviceAndContext();
	void initSwapChain();
	void setupPShader(TCHAR fileName[]);
	void setupVShader(TCHAR fileName[]);

	void draw();
};
