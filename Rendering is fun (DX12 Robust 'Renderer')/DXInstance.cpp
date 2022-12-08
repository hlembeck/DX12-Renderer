#include "DXInstance.h"

DXBase::DXBase() {}

DXBase::~DXBase() {}

void DXBase::CreateDevice(IDXGIFactory4* factory) {
    ComPtr<IDXGIAdapter1> adapter;
    GetAdapter(factory, &adapter);

    ThrowIfFailed(D3D12CreateDevice(
        adapter.Get(),
        D3D_FEATURE_LEVEL_12_0,
        IID_PPV_ARGS(&m_device)
    ));
}

void DXBase::GetAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter) {
	UINT i = 0;
	ComPtr<IDXGIAdapter1> adapter;
	while (pFactory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND) {
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);
		//Don't use the basic render driver software adapter.
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			continue;
		}
		*ppAdapter = adapter.Detach();
		return;
	}
	*ppAdapter = NULL;
}

WindowInterface::WindowInterface(UINT width, UINT height) : m_width(width), m_height(height) {
    m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

WindowInterface::~WindowInterface() {};

float WindowInterface::GetAspectRatio() {
    return m_aspectRatio;
}

DXWindowBase::DXWindowBase() {}

DXWindowBase::~DXWindowBase() {

}

void DXWindowBase::CreateRenderTargets(ID3D12DescriptorHeap* rtvHeap, UINT descriptorSize) {

    m_viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(WindowInterface::m_width), static_cast<float>(WindowInterface::m_height));
    m_scissorRect = CD3DX12_RECT(0, 0, static_cast<LONG>(WindowInterface::m_width), static_cast<LONG>(WindowInterface::m_height));

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
    // Create a RTV for each frame.
    for (UINT i = 0; i < NUMFRAMES; i++)
    {
        ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])));
        m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
        m_renderTargets[i]->SetName(L"Render Target");
        rtvHandle.Offset(1, descriptorSize);
    }
}

void DXWindowBase::CreateSwapChain(HWND hWnd) {
    //Create Device
    ComPtr<IDXGIFactory4> factory;
    UINT dxgiFactoryFlags = 0;
    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));
    DXBase::CreateDevice(factory.Get());

    D3D12_COMMAND_QUEUE_DESC queueDesc = { D3D12_COMMAND_LIST_TYPE_DIRECT , D3D12_COMMAND_QUEUE_FLAG_NONE };
    ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

    // Describe and create the swap chain.
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {
        m_width,
        m_height,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        FALSE,
        {1,0}, //sample desc
        DXGI_USAGE_RENDER_TARGET_OUTPUT,
        NUMFRAMES,
        DXGI_SCALING_STRETCH, //Scaling stretch
        DXGI_SWAP_EFFECT_FLIP_DISCARD,

    };
    //printf("%p\n", m_device.Get());

    ComPtr<IDXGISwapChain1> swapChain;
    //IDXGIFactory2 necessary for CreateSwapChainForHwnd
    ThrowIfFailed(factory->CreateSwapChainForHwnd(
        m_commandQueue.Get(),
        hWnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain
    ));

    //DXGI won't respond to alt-enter - WndProc is responsible.
    ThrowIfFailed(factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

    ThrowIfFailed(swapChain.As(&m_swapChain)); //Original swap chain interface now represented by m_swapChain
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

void DXWindowBase::CreateFence() {
    ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
    m_fenceValue = 1;

    // Create an event handle to use for frame synchronization.
    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (m_fenceEvent == nullptr) {
        ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
    }

    WaitForPreviousFrame();
}

HRESULT DXWindowBase::Present() {
    return m_swapChain->Present(1, 0);
}

void DXWindowBase::WaitForPreviousFrame() {
    const UINT64 fence = m_fenceValue;
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), fence));
    m_fenceValue++;
    if (m_fence->GetCompletedValue() < fence) {
        ThrowIfFailed(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}