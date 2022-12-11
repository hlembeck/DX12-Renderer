#include "Renderer.h"
#include "Application.h"

Renderer::Renderer(UINT width, UINT height) : DXWindowBase(width, height) {}

Renderer::~Renderer() {}

void Renderer::OnInit() {
	CreateSwapChain(Application::GetHWND());
    CreateFence();
    BasicDraw::Load();

    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
        NUMFRAMES,
        D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        0 //Single Adapter
    };
    ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));
    m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);


    DXWindowBase::CreateRenderTargets(m_rtvHeap.Get(),m_rtvDescriptorSize);


    ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
    ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), NULL, IID_PPV_ARGS(&m_commandList)));
    ThrowIfFailed(m_commandList->Close());
}

void Renderer::SetCameraConstants(CameraShaderConstants constants) {
    BasicDraw::m_rootConstants = constants;
}

void Renderer::OnRender(BasicRenderObject* pObjects, UINT numObjects, D3D12_GPU_VIRTUAL_ADDRESS lightBufferView) {
    { //Reset the command list, and set it up for the render target
        ThrowIfFailed(m_commandAllocator->Reset());
        ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), NULL));
        m_commandList->RSSetViewports(1, &m_viewport);
        m_commandList->RSSetScissorRects(1, &m_scissorRect);
        CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        m_commandList->ResourceBarrier(1, &resourceBarrier);
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
        m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
        const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

        BasicDraw::Draw(m_commandList.Get(), pObjects, numObjects, lightBufferView);

        //Setup for presenting
        resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        m_commandList->ResourceBarrier(1, &resourceBarrier);
        ThrowIfFailed(m_commandList->Close());
    }

    // Execute the command list.
    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    DXWindowBase::m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // Present the frame.
    Present();
    WaitForPreviousFrame();
}

void Renderer::OnDestroy() {}