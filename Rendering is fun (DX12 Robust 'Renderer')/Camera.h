#pragma once
#include "Shared.h"

/*
Implementation based on Camera class from Frank Luna's "Introduction to 3D Game Programming with DirectX 12"
*/

using namespace DirectX;
class Camera {
public:
	Camera();
	Camera(float fovY, float aspectRatio, float nearZ, float farZ);
	~Camera();

	virtual void OnInit(float fovY, float aspectRatio, float nearZ, float farZ);

	void SetLens(float fovV, float ar, float zn, float zf);
	void Move(XMFLOAT3 dist);

	void Pitch(float angle);
	void RotateWorldY(float angle);

	void UpdateViewMatrix();
	XMMATRIX GetViewMatrix();
	XMMATRIX GetViewProjectionMatrix();

	XMFLOAT4 GetPosition();
	XMFLOAT4 GetDirection();
private:
	//Position in world space
	XMFLOAT3 m_position;
	//Coordinate system with respect to camera orientation
	XMFLOAT3 m_right;
	XMFLOAT3 m_up;
	XMFLOAT3 m_direction;

	float m_nearZ;
	float m_farZ;
	float m_aspectRatio;
	float m_fovY;
	float m_nearWindowHeight;
	float m_farWindowHeight;

	//Matrices used per render
	XMMATRIX m_viewMatrix;
	XMMATRIX m_projectionMatrix;
};