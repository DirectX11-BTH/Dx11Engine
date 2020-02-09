#include "Camera.h"
DirectX::XMVECTOR Camera::cameraPosition = DirectX::XMVectorSet(0.f, 0.0f, 4.f, 0.0f);
DirectX::XMVECTOR Camera::cameraTarget =  DirectX::XMVectorSet(0.f, 0.0f, 1.f, 0.0);
DirectX::XMVECTOR Camera::cameraUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
DirectX::XMVECTOR Camera::cameraRight = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
DirectX::XMVECTOR Camera::cameraForward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

DirectX::XMMATRIX Camera::cameraProjectionMatrix;
DirectX::XMMATRIX Camera::cameraView;

float Camera::zTranslation = 0.f;
float Camera::xTranslation = 0.f;
float Camera::yTranslation = 0.f;

float Camera::roll, Camera::pitch, Camera::yaw = 0.f;
using namespace DirectX;

Camera::Camera()
{
}

void Camera::updateCamera()
{
	XMMATRIX cameraRotationMatrix = XMMatrixRotationRollPitchYaw(Camera::pitch, Camera::yaw, 0.f);
	Camera::cameraTarget = XMVector3TransformCoord(XMVectorSet(0, 0, 1, 0), cameraRotationMatrix); //Spinning the target around us
	Camera::cameraTarget = XMVector3Normalize(Camera::cameraTarget);

	//XMMATRIX tempYMatrix;
	XMMATRIX tempYMatrix = XMMatrixRotationY(Camera::yaw); //left to right
	XMMATRIX tempXMatrix = XMMatrixRotationX(Camera::pitch); //look up and down
	 
	Camera::cameraRight = XMVector3TransformCoord(XMVectorSet(1, 0, 0, 0), tempYMatrix); //To move sideways (X axis)
	Camera::cameraRight = XMVector3Normalize(Camera::cameraRight);


	Camera::cameraUp = XMVector3TransformCoord(Camera::cameraUp, tempYMatrix);
	Camera::cameraUp = XMVector3Normalize(Camera::cameraUp);

	Camera::cameraForward = XMVector3TransformCoord(XMVectorSet(0, 0, 1, 0), tempYMatrix);
	Camera::cameraForward = XMVector3Normalize(Camera::cameraForward);

	Camera::cameraPosition += Camera::cameraRight * xTranslation;
	Camera::cameraPosition += Camera::cameraForward * zTranslation;
	Camera::cameraPosition += Camera::cameraUp * yTranslation;

	Camera::cameraTarget = Camera::cameraPosition + Camera::cameraTarget * -1;//Camera::cameraForward * -2;//Camera::cameraTarget;

	cameraView = DirectX::XMMatrixLookAtLH(Camera::cameraPosition, Camera::cameraTarget, Camera::cameraUp);
}
