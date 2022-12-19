#pragma once
#include "BasicDraw.h"
#include "Camera.h"
#include "Object.h"

class Application;

//Resopnsible for rendering and presenting frames.
class Renderer :
	//public virtual WindowInterface,
	public DXWindowBase, //For swap chain
	private BasicDraw
{
public:
	Renderer(UINT width, UINT height);
	~Renderer();

	void OnInit();
	void OnRender(BasicRenderObject* pObjects, UINT numObjects, D3D12_GPU_VIRTUAL_ADDRESS lightBufferView);
	void OnDestroy();
	void SetCameraConstants(CameraShaderConstants constants);
private:
	void CreateDepthBuffer();
	friend Application;

	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	UINT m_rtvDescriptorSize;

	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	ComPtr<ID3D12Resource> m_depthBuffer;
	ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
	ComPtr<ID3D12PipelineState> m_pipelineState; //Don't want a PSO, but depth buffer clearing requires one
};