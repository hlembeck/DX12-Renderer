#pragma once
#include "DXInstance.h"

struct PlacedObject {
	ComPtr<ID3D12Resource> m_vertices;
	UINT m_numVertices;
	XMMATRIX m_worldMatrix;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
};

struct RootConstants {
	XMMATRIX cameraMatrix;
	XMFLOAT4 viewDirection;
	XMFLOAT4 viewPosition;
};

constexpr UINT MAXLIGHTS = 3;

class BasicDraw : private virtual DXBase {
public:
	BasicDraw();
protected:
	void Load(); //Must be called after device creation.
	void LoadObject(XMFLOAT4* vertices, UINT numVertices, XMMATRIX worldMatrix);
	void Draw(ID3D12GraphicsCommandList* pCommandList, RootConstants constants, PointLight* lights, UINT numLights);
private:
	ComPtr<ID3D12PipelineState> m_pipelineState;
	ComPtr<ID3D12RootSignature> m_rootSignature; //Ideally, inherits a root signature from elsewhere, or Renderer (that inherits this class) contains the root signature.
	void CreatePipelineState();
	void CreateRootSignature();
	void CreateLightBuffer();

	ComPtr<ID3D12Resource> m_lightBuffer;

	std::vector<PlacedObject> m_objects;
};