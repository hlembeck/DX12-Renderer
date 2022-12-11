#pragma once
#include "Light.h"
#include "Terrain.h"

class Scene :
	public Lights,
	public Terrain
{
public:
	Scene();
	~Scene();
};