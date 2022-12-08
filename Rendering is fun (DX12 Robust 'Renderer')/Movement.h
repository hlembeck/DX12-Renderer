#pragma once
#include "Shared.h"

class Movement {
public:
	Movement();
	~Movement();
private:
	XMFLOAT3 m_position;
	XMFLOAT3 m_linearVelocity;
};