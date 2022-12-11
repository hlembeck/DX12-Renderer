#pragma once
#include "Shared.h"

constexpr UINT NUMFRAMES = 2;

class RandomGenerator {
	std::mt19937 m_gen;
	std::uniform_real_distribution<float> m_dist;
protected:
	float Get();
public:
	RandomGenerator();
};

class DXBase : public virtual RandomGenerator {
public:
	DXBase();
	~DXBase();
protected:
	void GetAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter);
	void CreateDevice(IDXGIFactory4* factory);
	ComPtr<ID3D12Device> m_device;
};

class WindowInterface {
	float m_aspectRatio;
public:
	WindowInterface(UINT width, UINT height);
	~WindowInterface();
	virtual void OnInit() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;
	virtual void OnKeyDown(WPARAM wParam) = 0;
	virtual void OnKeyUp(WPARAM wParam) = 0;
	virtual void OnDestroy() = 0;
	virtual void SetCursorPoint(POINT p, HCURSOR hCursor) = 0;
protected:
	UINT m_width;
	UINT m_height;
	float GetAspectRatio();
};

class DXWindowBase :
	public virtual DXBase,
	public WindowInterface
{
public:
	DXWindowBase(UINT width, UINT height);
	virtual ~DXWindowBase();
protected:
	void CreateRenderTargets(ID3D12DescriptorHeap* rtvHeap, UINT descriptorSize);
	void CreateSwapChain(HWND hWnd);
	void CreateFence();
	HRESULT Present();
	void WaitForPreviousFrame();
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	ComPtr<ID3D12Resource> m_renderTargets[NUMFRAMES];
	UINT m_frameIndex;
	CD3DX12_RECT m_scissorRect;
	CD3DX12_VIEWPORT m_viewport;
private:
	ComPtr<IDXGISwapChain3> m_swapChain;
	//Fence
	UINT64 m_fenceValue;
	HANDLE m_fenceEvent;
	ComPtr<ID3D12Fence> m_fence;
};