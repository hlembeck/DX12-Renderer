#pragma once
#include "DXInstance.h"
#include "BasicDraw.h"
#include "Camera.h"

class Application;

//Resopnsible for rendering and presenting frames.
class Renderer :
	public virtual WindowInterface,
	private DXWindowBase, //For swap chain
	private BasicDraw
{
public:
	Renderer(UINT width, UINT height);
	~Renderer();

	void OnInit() final;
	void OnUpdate() final;
	void OnRender() final;
	void OnDestroy() final;
	void OnKeyDown(WPARAM wParam) final;
	void OnKeyUp(WPARAM wParam) final;
	void SetCursorPoint(POINT p, HCURSOR hCursor) final;

	/*void LoadGraphicsPipeline();
	void LoadAssets();*/
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

	Camera m_camera;
	XMFLOAT3 m_linearVelocity;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_time;
	BOOL m_inputCaptured;
	POINT m_cursorPos;
	float m_mouseScaleFactor; //Sets angular velocity, will use fov to do this later
	HCURSOR m_hCursor;
};