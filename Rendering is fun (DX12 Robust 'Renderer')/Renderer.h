#pragma once
#include "DXInstance.h"
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
	//void SetCursorPoint(POINT p, HCURSOR hCursor);

	/*void LoadGraphicsPipeline();
	void LoadAssets();*/
	void SetCameraConstants(CameraShaderConstants constants);
private:
	friend Application;
	/*void WaitForPreviousFrame();
	void FillCommandList();
	void CreateRootSignature();
	void CreatePipelineState();*/

	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	UINT m_rtvDescriptorSize;

	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
};