#include "Renderer.h"
#include "Application.h"

Renderer::Renderer(UINT width, UINT height) : WindowInterface(width, height), m_linearVelocity({ 0.0f,0.0f,0.0f }), m_mouseScaleFactor(1.0f) {
    m_camera = Camera(45.0f, GetAspectRatio(), 0.1f, 100.0f);
    m_time = std::chrono::high_resolution_clock::now();
    m_inputCaptured = FALSE;
}

Renderer::~Renderer() {}

void Renderer::OnInit() {
	CreateSwapChain(Application::GetHWND());
    CreateFence();
    BasicDraw::Load();


    {
        XMFLOAT4 vertices[12] = {
            {-5.0f,-5.0f,0.0f,1.0f}, {0.0f,0.0f,-1.0f,0.0f},
            {-5.0f,5.0f,0.0f,1.0f}, {0.0f,0.0f,-1.0f,0.0f},
            {5.0f,5.0f,0.0f,1.0f}, {0.0f,0.0f,-1.0f,0.0f},

            {-5.0f,-5.0f,0.0f,1.0f}, {0.0f,0.0f,-1.0f,0.0f},
            {5.0f,5.0f,0.0f,1.0f}, {0.0f,0.0f,-1.0f,0.0f},
            {5.0f,-5.0f,0.0f,1.0f}, {0.0f,0.0f,-1.0f,0.0f},
        };
        BasicDraw::LoadObject(vertices, 6, XMMatrixIdentity());
    }


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

void Renderer::OnUpdate() {
    std::chrono::time_point<std::chrono::high_resolution_clock> curr = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> diff = curr - m_time;
    float elapsedTime = diff.count();
    m_time = curr;
    m_camera.Move({ m_linearVelocity.x * elapsedTime,0.0f ,m_linearVelocity.z * elapsedTime });
    m_camera.MoveUp(m_linearVelocity.y * elapsedTime);

    if (m_inputCaptured) {
        POINT cursorPos = {};
        GetCursorPos(&cursorPos);
        m_camera.Pitch(((cursorPos.y - m_cursorPos.y) / m_mouseScaleFactor) * elapsedTime);
        m_camera.RotateWorldY(((cursorPos.x - m_cursorPos.x) / m_mouseScaleFactor) * elapsedTime);
        SetCursorPos(m_cursorPos.x, m_cursorPos.y);
    }
}

void Renderer::OnRender() {
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

        {
            PointLight lights[MAXLIGHTS] = {
                { {3.0f,1.0f,-3.0f,0.0f}, {0.5f,0.0f,1.0f,1.0f} },
                { {-3.0f,1.0f,-3.0f,0.0f}, {1.0f,1.0f,0.0f,1.0f} },
                { {1.0f, -3.0f, -5.0f, 0.0f}, {0.0f,1.0f,1.0f,1.0f} },
            };
            RootConstants constants = { XMMatrixTranspose(m_camera.GetViewProjectionMatrix()), m_camera.GetDirection(), m_camera.GetPosition()};
            BasicDraw::Draw(m_commandList.Get(), constants, lights, 3);
        }


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

void Renderer::OnKeyDown(WPARAM wParam) {
    if (wParam == VK_ESCAPE) {
        if (m_inputCaptured) {
            m_inputCaptured = FALSE;
            SetCursor(m_hCursor);
        }
        else {
            m_inputCaptured = TRUE;
            SetCursor(NULL);
        }
        return;
    }
    if (m_inputCaptured) {
        switch (wParam) {
        case 0x57: //'w' key
            m_linearVelocity.z = 5.0f;
            return;
        case 0x41: //'a' key
            m_linearVelocity.x = -5.0f;
            return;
        case 0x53: //'s' key
            m_linearVelocity.z = -5.0f;
            return;
        case 0x44: //'d' key
            m_linearVelocity.x = 5.0f;
            return;
        case VK_SPACE:
            m_linearVelocity.y = 5.0f;
            return;
        case VK_SHIFT:
            m_linearVelocity.y = -5.0f;
            return;
        }
    }
}

void Renderer::OnKeyUp(WPARAM wParam) {
    if (m_inputCaptured) {
        switch (wParam) {
        case 0x57: //'w' key
            if (m_linearVelocity.z > 0)
                m_linearVelocity.z = 0.0f;
            return;
        case 0x41: //'a' key
            if (m_linearVelocity.x < 0)
                m_linearVelocity.x = 0.0f;
            return;
        case 0x53: //'s' key
            if (m_linearVelocity.z < 0)
                m_linearVelocity.z = 0.0f;
            return;
        case 0x44: //'d' key
            if (m_linearVelocity.x > 0)
                m_linearVelocity.x = 0.0f;
            return;
        case VK_SPACE:
            if (m_linearVelocity.y > 0)
                m_linearVelocity.y = 0.0f;
            return;
        case VK_SHIFT:
            if (m_linearVelocity.y < 0)
                m_linearVelocity.y = 0.0f;
            return;
        }
    }
}

void Renderer::OnDestroy() {}

void Renderer::SetCursorPoint(POINT p, HCURSOR hCursor) {
    m_cursorPos = p;
    m_mouseScaleFactor = 20.0f;
    m_hCursor = hCursor;
    SetCursor(m_hCursor);
}