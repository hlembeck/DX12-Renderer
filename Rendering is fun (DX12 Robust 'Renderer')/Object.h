#pragma once
#include "DXInstance.h"

class SimpleMesh : public BasicRenderObject {
public:
	SimpleMesh();
	void Load(ID3D12Device* device, SimpleVertex* vertices, UINT numVertices, XMMATRIX worldMatrix);
private:
	XMMATRIX m_worldMatrix;
	ComPtr<ID3D12Resource> m_vertexBuffer;
};