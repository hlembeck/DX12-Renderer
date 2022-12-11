#pragma once
#include "Object.h"

class Terrain : public virtual DXBase {
public:
	Terrain();

	void Load();

	BasicRenderObject* GetRenderObject();
private:
	SimpleMesh m_mesh;
};