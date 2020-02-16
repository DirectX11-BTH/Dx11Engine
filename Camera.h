#pragma once
#include <DirectXHelpers.h>
#include <SimpleMath.h>
#include <math.h>
#include "TerrainGenerator.h"

const bool WALK_ON_TERRAIN = false;

class Camera
{
private:

public:
	Camera();


	static DirectX::XMVECTOR cameraPosition; //-8f to shoot the camera 'back' a bit //Is set in camera.cpp as static
	static DirectX::XMVECTOR cameraTarget;

	static DirectX::XMVECTOR cameraUp;
	static DirectX::XMVECTOR cameraForward;
	static DirectX::XMVECTOR cameraRight;

	static float zTranslation;
	static float xTranslation;
	static float yTranslation;

	static DirectX::XMMATRIX cameraProjectionMatrix; //Contains far plane, aspect ratio etc

	static float roll, pitch, yaw;


	static DirectX::XMMATRIX cameraView;

	static void updateCamera();
};