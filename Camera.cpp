#include "Camera.h"
DirectX::XMVECTOR Camera::cameraPosition = DirectX::XMVectorSet(1.0f, 1.0f, -1.2f, 0.0f);
DirectX::XMVECTOR Camera::cameraTarget =  DirectX::XMVectorSet(0.f, 0.0f, 1.f, 0.0);
DirectX::XMVECTOR Camera::cameraUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
DirectX::XMMATRIX Camera::cameraProjectionMatrix;
DirectX::XMMATRIX Camera::cameraView;

Camera::Camera()
{
}
