#pragma once
#include "Camera.h"
#include "Movement.h"

class Player :
	private Camera,
	private Movement
{
public:
	Player(float fovY, float aspectRatio, float nearZ, float farZ);
	~Player();
private:
	XMFLOAT3 m_position;
};