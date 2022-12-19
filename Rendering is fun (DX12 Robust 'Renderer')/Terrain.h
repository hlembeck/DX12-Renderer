#pragma once
#include "Object.h"
#include "HeightmapTerrainGen.h"

class Terrain :
	public virtual DXBase
{
public:
	Terrain();

	void Load();
	void Update(XMFLOAT4 position);

	BasicRenderObject GetRenderObject();
private:
	SimpleMesh m_mesh;
	HeightmapTerrain m_terrain;
};