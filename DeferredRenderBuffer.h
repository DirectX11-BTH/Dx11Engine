#pragma once
#include <dxgi.h>
#include <d3d11.h>
#include "DxHandler.h"


class DeferredRenderBuffer
{
private:
    int texWidth = 0;
    int texHeight = 0;

public:
    ~DeferredRenderBuffer();

    ID3D11Texture2D* renderTargetTexture;
    ID3D11RenderTargetView* renderTargetView;

    ID3D11ShaderResourceView* shaderResourceView;
    //D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

    DeferredRenderBuffer(int textureWidth, int textureHeight);
    void init(int texWidth, int texHeight);
    

};