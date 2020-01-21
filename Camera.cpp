#include "Camera.h"
DirectX::XMVECTOR Camera::cameraPosition = DirectX::XMVectorSet(0.0f, 0.0f, -4.f, 0.0f);
DirectX::XMVECTOR Camera::cameraTarget =  DirectX::XMVectorSet(3.f, 0.0f, 0.f, 0.0f);
DirectX::XMVECTOR Camera::cameraUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
DirectX::XMMATRIX Camera::cameraProjectionMatrix = DirectX::XMMATRIX();
DirectX::XMMATRIX Camera::cameraView = DirectX::XMMATRIX();

Camera::Camera()
{
}
