#pragma once
#include "Object.h"
#include "HeightmapTerrainGen.h"

class Terrain :
	public virtual DXBase
{
public:
	Terrain();

	void Load(ID3D12CommandQueue* commandQueue);
	void Update(XMFLOAT4 position, ID3D12CommandQueue* commandQueue);

	BasicRenderObject GetRenderObject();
private:
	SimpleMesh m_mesh;
	HeightmapTerrain m_terrain;
};