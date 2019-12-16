#include "DXHandler.h"

ID3D11Device* DxHandler::devicePtr = nullptr;
ID3D11DeviceContext* DxHandler::contextPtr = nullptr;
HINSTANCE DxHandler::hInstance = HINSTANCE();

IDXGISwapChain* DxHandler::swapChainPtr = nullptr;
ID3D11RenderTargetView* DxHandler::renderTargetPtr = nullptr;
DXGI_SWAP_CHAIN_DESC DxHandler::swapDesc = DXGI_SWAP_CHAIN_DESC{ 0 };
ID3DBlob* DxHandler::vertexShaderBuffer = nullptr;
ID3DBlob* DxHandler::pixelShaderBuffer = nullptr;

ID3D11PixelShader* DxHandler::pixelPtr = nullptr;
ID3D11VertexShader* DxHandler::vertexPtr = nullptr;
ID3D11InputLayout* DxHandler::input_layout_ptr = nullptr;

void DxHandler::initalizeDeviceContextAndSwapChain()
{
	//ID3D11DeviceContext* contextPtr = contextPtr;
	//ID3D11Device* devicePtr = devicePtr;

	HRESULT succ = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&DxHandler::swapDesc,
		&DxHandler::swapChainPtr,
		&DxHandler::devicePtr,
		nullptr,
		&DxHandler::contextPtr
	);

	assert(SUCCEEDED(succ));
	assert(devicePtr != nullptr);

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	ID3D11RasterizerState* rasterizerState;
	
	devicePtr->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
	contextPtr->RSSetState(rasterizerState);

	wrl::ComPtr<ID3D11Texture2D> backBufferPtr;// not sure how this works but backbufferptr wont work with normal ID3D11ResourcePtr
	(swapChainPtr->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBufferPtr));
	HRESULT rendertargetSucc = devicePtr->CreateRenderTargetView(backBufferPtr.Get(), nullptr, &renderTargetPtr);
	assert(SUCCEEDED(rendertargetSucc));
}

void DxHandler::configureSwapChain(HWND& hWnd)
{
	D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE; //The hardware driver we'd like to use.
	D3D_FEATURE_LEVEL version[] = { D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0 }; //Which version we're running
	// CREATING OUR CHAIN DESC
	
	//BUFFERDESC
	DxHandler::swapDesc.BufferDesc.Width = 0;//widthOfScreen;
	DxHandler::swapDesc.BufferDesc.Height = 0;//heightOfScreen;
	DxHandler::swapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;// doesnt work with 32 for some reason
	DxHandler::swapDesc.BufferDesc.RefreshRate.Numerator = 60;//can be set to 0 for no max
	DxHandler::swapDesc.BufferDesc.RefreshRate.Denominator = 0;
	DxHandler::swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	DxHandler::swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	//SAMPLEDESC
	DxHandler::swapDesc.SampleDesc.Count = 1;
	DxHandler::swapDesc.SampleDesc.Quality = 0;
	//END SAMPLEDESC

	//GENERAL SWAPDESC
	DxHandler::swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	DxHandler::swapDesc.BufferCount = 2; // might want 1 here. Not sure
	DxHandler::swapDesc.OutputWindow = hWnd;
	DxHandler::swapDesc.Windowed = TRUE;
	DxHandler::swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	DxHandler::swapDesc.Flags = 0;
		//Checks for debug mode or not/////////
	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	//END GENERAL SWAPDESC

	/*HRESULT succ = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&DxHandler::swapDesc,
		&DxHandler::swapChainPtr,
		&DxHandler::devicePtr,
		nullptr,
		&DxHandler::contextPtr
	);
		std::string s = std::to_string(succ);
	//MessageBox(NULL, LPCWSTR(s.c_str()), L"Blah blah", MB_YESNO);//notice the L is removed

	assert(succ == S_OK);*/
	//assert(contextPtr != nullptr);
	//assert(swapChainPtr != nullptr);
	////

}
void DxHandler::setupInputLayout()
{
	//ID3D11InputLayout* input_layout_ptr = NULL;

	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	HRESULT inputLayoutSucc = devicePtr->CreateInputLayout
	(
		inputDesc, ARRAYSIZE(inputDesc),
		vertexShaderBuffer->GetBufferPointer(), //TO DO
		vertexShaderBuffer->GetBufferSize(), // TO DO
		&input_layout_ptr
	);
	assert(SUCCEEDED(inputLayoutSucc));
}
ID3D11Buffer* DxHandler::createVertexBuffer(Mesh& mesh)
{
	ID3D11Buffer* vertexBufferPtr = NULL;

	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.ByteWidth = sizeof(float) * FLOATS_PER_VERTEX * mesh.vertices.size(); // TO DO?
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subResData = D3D11_SUBRESOURCE_DATA{0};
	subResData.pSysMem = mesh.vertices.data();
	subResData.SysMemPitch = FLOATS_PER_VERTEX * sizeof(float) * mesh.vertices.size();

	MSG msg;

	HRESULT succ2 = devicePtr->CreateBuffer(&bufferDesc, &subResData, &vertexBufferPtr);
	assert(succ2 == S_OK);

	mesh.vertexBuffer = vertexBufferPtr;
	
	return vertexBufferPtr;
}
void DxHandler::setCullingMode(D3D11_CULL_MODE mode)
{
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = mode;
	ID3D11RasterizerState* rasterizerState;

	devicePtr->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
	contextPtr->RSSetState(rasterizerState);
}

void DxHandler::setupPShader(const wchar_t fileName[])
{
	ID3DBlob* errorMessage;

	HRESULT pixelShaderSucc = D3DCompileFromFile(
		fileName,
		nullptr,
		nullptr,//D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",//"ColorVertexShader",
		"ps_5_0", //Pixel shader
		0,
		0,
		&DxHandler::pixelShaderBuffer,
		&errorMessage
	);
	assert(SUCCEEDED(pixelShaderSucc));

	HRESULT createPShaderSucc = devicePtr->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelPtr);
	assert(SUCCEEDED(createPShaderSucc));
}

void DxHandler::setupVShader(const wchar_t fileName[])
{
	ID3DBlob* errorMessage;
	
	HRESULT	vertShaderSucc = D3DCompileFromFile(
		fileName,
		nullptr,
		nullptr,//D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",//"ColorVertexShader",
		"vs_5_0", //Vertex shader
		0,
		0,
		&DxHandler::vertexShaderBuffer,
		&errorMessage
	);

	HRESULT createVShaderSucc = devicePtr->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexPtr);
	assert(SUCCEEDED(createVShaderSucc));
}

void DxHandler::draw(EngineObject& drawObject) //Only draws one mesh, woops TO DO
{
	UINT stride = (UINT)sizeof(float) * FLOATS_PER_VERTEX;
	UINT offset = 0u;

	//for (int i = 0; i < drawObject.meshes.size(); i++)
	//{
	DxHandler::contextPtr->IASetVertexBuffers(0, 1, &drawObject.meshes.at(0).vertexBuffer,
		&stride, &offset);
	DxHandler::contextPtr->Draw(6, 0);
	//}

	std::cout << "DRAWING" << std::endl << "Nr of verts:\t" << drawObject.meshes.at(0).vertices.size() << std::endl;
	//swapChainPtr->Present(1, 0);
}
