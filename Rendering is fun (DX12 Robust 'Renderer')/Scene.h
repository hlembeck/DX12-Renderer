#pragma once
#include "Light.h"
#include "Terrain.h"

class Scene :
	public LightPerspectiveDepthMap,
	public Terrain
{
public:
	Scene();

	void Update(float elapsedTime, XMFLOAT4 position);

	~Scene();
};