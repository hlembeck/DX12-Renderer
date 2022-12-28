#pragma once
#include "Shared.h"

struct CubeMapCameras {
	XMMATRIX xP; //x "Plus" (x>0)
	XMMATRIX xM; //x "Minus" (x<0)
	XMMATRIX yP;
	XMMATRIX yM;
	XMMATRIX zP;
	XMMATRIX zM;
};

//Generates a layered cube-map for a given point light source. The cube-map stores distance from the light, as well as other surface/material attributes. This is used to generate more realistic specular reflections and refractions. See https://developer.nvidia.com/gpugems/gpugems3/part-iii-rendering/chapter-17-robust-multiple-specular-reflections-and-refractions
class LightPerspectiveDepthMap : public PointLight {
	const UINT m_resolution;
	const D3D12_VIEWPORT m_viewport;
	const D3D12_RECT m_scissorRect;
	ComPtr<ID3D12Resource> m_depthMap; //The cube map. Each resource is a layered depth map (see depth peeling).
	D3D12_CLEAR_VALUE m_clearVal;

	ComPtr<ID3D12Resource> m_depthBuffer; //Used as depth-stencil view for the depth map. In a future update to the object's properties, will use alongside a secondary "depth buffer" resource, that will allow for depth-peeling. Can allow for more accurate shading with transparrent objects.
	D3D12_CLEAR_VALUE m_depthClearVal;

	ComPtr<ID3D12PipelineState> m_pipelineState;
	ComPtr<ID3D12RootSignature> m_rootSignature; //Ideally, inherits a root signature from elsewhere, or Renderer (that inherits this class) contains the root signature.
	CubeMapCameras m_cubeMapCameras;


	ComPtr<ID3D12Resource> m_constantBuffer; //buffer for cameras


	//Handles to descriptors in heaps maintained by Game.
	D3D12_CPU_DESCRIPTOR_HANDLE m_dsvHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandle;


	void CreatePSO(ID3D12Device* const device); //Also create root signature. Will move the root signature elsewhere later

	void CreateCBV(ID3D12Device* const device, D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle); //Constant buffer view for the cameras (size of CubeMapCameras is too large to fit in root signature directly)

	void CreateSRV(ID3D12Device* const device, D3D12_CPU_DESCRIPTOR_HANDLE srvHandle);

	void CreateCameras();
public:
	LightPerspectiveDepthMap(XMFLOAT4 pos = {}, XMFLOAT4 color = {}, UINT resolution = 256);

	HRESULT Load(ID3D12Device* const device, D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle, D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, D3D12_CPU_DESCRIPTOR_HANDLE srvHandle);

	void Draw(ID3D12GraphicsCommandList* pCommandList, BasicRenderObject* pObjects, UINT numObjects);
	D3D12_GPU_VIRTUAL_ADDRESS GetCameras();
};