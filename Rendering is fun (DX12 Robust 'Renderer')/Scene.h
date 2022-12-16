#pragma once
#include "Light.h"
#include "Terrain.h"

class Scene :
	public Lights,
	public Terrain
{
public:
	Scene();

	void Update(float elapsedTime, XMFLOAT4 position, ID3D12CommandQueue* const commandQueue);

	~Scene();
};