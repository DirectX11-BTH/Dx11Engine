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
    DeferredRenderBuffer(); //Need to init this afterwards

    ID3D11Texture2D* renderTargetTexture;
    ID3D11RenderTargetView* renderTargetView;

    ID3D11ShaderResourceView* shaderResourceView;
    ID3D11UnorderedAccessView* unorderedAccessView;
    //D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

    DeferredRenderBuffer(int textureWidth, int textureHeight);
    void init(int texWidth, int texHeight);
    

};