#include "Camera.h"
DirectX::XMVECTOR Camera::cameraPosition = DirectX::XMVectorSet(200.0f, 200.f, 1.0f, 0);
DirectX::XMVECTOR Camera::cameraTarget =  DirectX::XMVectorSet(0.2000f, 0.f, 1.0f, 0.f);
DirectX::XMVECTOR Camera::cameraUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
DirectX::XMVECTOR Camera::cameraRight = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
DirectX::XMVECTOR Camera::cameraForward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

DirectX::XMMATRIX Camera::cameraProjectionMatrix;
DirectX::XMMATRIX Camera::cameraView;

bool Camera::WALK_ON_TERRAIN = false;

float Camera::zTranslation = 0.f;
float Camera::xTranslation = 0.f;
float Camera::yTranslation = 0.f;

float Camera::roll, Camera::pitch, Camera::yaw = 0.f;
using namespace DirectX;

Camera::Camera()
{
	if (WALK_ON_TERRAIN)
	{
		cameraPosition = DirectX::XMVectorSet(0.f, 0.0f, 0.f, 0.0f);//Init to start under/on terrain
	}
}

void Camera::updateCamera()
{
	/*
	This moves camera (in world space) & generates rotation matrices based on mouse movement (based on mouse delta position).
	Moves camera based on a multiplier to move it.
	*/

	if (WALK_ON_TERRAIN)
	{
		//set height of camera accordingly
		XMFLOAT4 cameraPosInFloats;
		XMStoreFloat4(&cameraPosInFloats, cameraPosition);
		int xpos = cameraPosInFloats.x / TerrainGenerator::scaling;
		int zpos = cameraPosInFloats.z / TerrainGenerator::scaling;	
		int whichVertex = (zpos *6* (TerrainGenerator::width-1)+(xpos*6)); //Returns vertex index based on which x and y position the camera is on. Always the 6th vertex of the quad.
		//This works because our terrain is grid based & matched with pixels from a texture.

		float heightOfTriangle = 0;

		if (whichVertex < TerrainGenerator::heightTerrain.vertices.size())
			heightOfTriangle = (TerrainGenerator::heightTerrain.vertices.at(whichVertex).y)+150.f; //Retrieves vertex height to set camera to (+ some offset).

		cameraPosition = DirectX::XMVectorSet(cameraPosInFloats.x, heightOfTriangle, cameraPosInFloats.z, 1.0f);
	}
	XMMATRIX cameraRotationMatrix = XMMatrixRotationRollPitchYaw(Camera::pitch, Camera::yaw, 0.f);
	Camera::cameraTarget = XMVector3TransformCoord(XMVectorSet(0, 0, 1, 0), cameraRotationMatrix); //Spinning the target around us
	Camera::cameraTarget = XMVector3Normalize(Camera::cameraTarget);

	XMMATRIX tempYMatrix = XMMatrixRotationY(Camera::yaw); //left to right, generates rotation matrix on y axis.
	XMMATRIX tempXMatrix = XMMatrixRotationX(Camera::pitch); //look up and down, generates rotation matrix on x axis.
	 
	Camera::cameraRight = XMVector3TransformCoord(XMVectorSet(1, 0, 0, 0), tempYMatrix); //To move sideways (X axis)
	Camera::cameraRight = XMVector3Normalize(Camera::cameraRight);

	Camera::cameraUp = XMVector3TransformCoord(Camera::cameraUp, tempYMatrix);
	Camera::cameraUp = XMVector3Normalize(Camera::cameraUp);

	Camera::cameraForward = XMVector3TransformCoord(XMVectorSet(0, 0, 1, 0), tempYMatrix);
	Camera::cameraForward = XMVector3Normalize(Camera::cameraForward);

	XMFLOAT4 cameraPosInFloats;
	XMStoreFloat4(&cameraPosInFloats, cameraPosition);
	cameraPosition = DirectX::XMVectorSet(cameraPosInFloats.x, cameraPosInFloats.y, cameraPosInFloats.z, 1.0f);


	Camera::cameraPosition += Camera::cameraRight * xTranslation;
	Camera::cameraPosition += Camera::cameraForward * zTranslation;
	Camera::cameraPosition += Camera::cameraUp * yTranslation;

	Camera::cameraTarget = Camera::cameraPosition + Camera::cameraTarget * -1;


	cameraView = DirectX::XMMatrixLookAtLH(Camera::cameraPosition, Camera::cameraTarget, Camera::cameraUp);
}
