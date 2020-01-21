#pragma once
#include <DirectXHelpers.h>
#include <SimpleMath.h>
#include <math.h>

class Camera
{
private:

public:
	Camera();


	static DirectX::XMVECTOR cameraPosition; //-8f to shoot the camera 'back' a bit //Is set in camera.cpp as static
	static DirectX::XMVECTOR cameraTarget;
	static DirectX::XMVECTOR cameraUp;
	static DirectX::XMMATRIX cameraProjectionMatrix; //Contains far plane, aspect ratio etc


	static DirectX::XMMATRIX cameraView;
};