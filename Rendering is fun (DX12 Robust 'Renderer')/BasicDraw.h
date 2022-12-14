#pragma once
#include "DXInstance.h"

//struct RootConstants {
//	XMMATRIX cameraMatrix;
//	XMFLOAT4 viewDirection;
//	XMFLOAT4 viewPosition;
//};

using RootConstants = CameraShaderConstants;

//constexpr UINT MAXLIGHTS = 3;

class BasicDraw : private virtual DXBase {
public:
	BasicDraw();
protected:
	void Load(); //Must be called after device creation.
	void Draw(ID3D12GraphicsCommandList* pCommandList, BasicRenderObject* pObjects, UINT numObjects, D3D12_GPU_VIRTUAL_ADDRESS lightBufferView);
	void DrawWithShadow(ID3D12GraphicsCommandList* pCommandList, BasicRenderObject* pObjects, UINT numObjects, D3D12_GPU_DESCRIPTOR_HANDLE depthCubeMap, D3D12_GPU_VIRTUAL_ADDRESS cameraView, PointLight pl);
	RootConstants m_rootConstants;
private:
	//Draw with robust light:
	ComPtr<ID3D12PipelineState> m_robustLightPipelineState;



	ComPtr<ID3D12PipelineState> m_pipelineState;
	ComPtr<ID3D12RootSignature> m_rootSignature; //Ideally, inherits a root signature from elsewhere, or Renderer (that inherits this class) contains the root signature.
	void CreatePipelineState();
	void CreateRootSignature();
};