#include "LightPerspectiveDepthGen.h"
#include "TextureGen.h"

LightPerspectiveDepthMap::LightPerspectiveDepthMap(XMFLOAT4 pos, XMFLOAT4 color, UINT resolution) : PointLight({ pos, color }), m_resolution(resolution), m_viewport({0.0f,0.0f,(float)resolution,(float)resolution,0.0f,1.0f}), m_scissorRect({ 0, 0, (LONG)resolution, (LONG)resolution }) {}

HRESULT LightPerspectiveDepthMap::Load(ID3D12Device* const device, D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle, D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, D3D12_CPU_DESCRIPTOR_HANDLE srvHandle) {

    //LIGHT TESTING
    /*position = { 0.0f,5.0f,0.0f,1.0f };
    color = { 1.0f,0.0f,0.0f,1.0f };*/


    m_dsvHandle = dsvHandle;
    m_rtvHandle = rtvHandle;

    HRESULT ret;

    m_clearVal.Format = DXGI_FORMAT_R32_FLOAT;
    m_clearVal.Color[0] = 1.0f;

    m_depthClearVal.Format = DXGI_FORMAT_D32_FLOAT;
    m_depthClearVal.DepthStencil = { 1.0f,0 };

    ret = GetTexture2D(m_depthMap, device, m_resolution, m_resolution, 6, DXGI_FORMAT_R32_FLOAT, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, NULL, &m_clearVal);
    ret |= GetTexture2D(m_depthBuffer, device, m_resolution, m_resolution, 6, DXGI_FORMAT_D32_FLOAT, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,NULL, &m_depthClearVal);
    if (SUCCEEDED(ret)) {

        { //Depth map
            D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {
                DXGI_FORMAT_R32_FLOAT,
                D3D12_RTV_DIMENSION_TEXTURE2DARRAY
            };
            rtvDesc.Texture2DArray = {
                0,
                0,
                6,
                0
            };
            device->CreateRenderTargetView(m_depthMap.Get(), &rtvDesc, rtvHandle);
        }

        { //Depth buffer
            D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
            depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
            depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
            depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

            depthStencilDesc.Texture2DArray = { 0,0,6 };

            m_depthBuffer->SetName(L"Depth Buffer: Light Perspective");

            device->CreateDepthStencilView(m_depthBuffer.Get(), &depthStencilDesc, dsvHandle);
        }

    }
    CreateCBV(device, cbvHandle);
    CreateSRV(device, srvHandle);
    CreatePSO(device);
    CreateCameras();
    return ret;
}

void LightPerspectiveDepthMap::Draw(ID3D12GraphicsCommandList* pCommandList, BasicRenderObject* pObjects, UINT numObjects) {
    CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_depthMap.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
    pCommandList->ResourceBarrier(1, &resourceBarrier);
    pCommandList->OMSetRenderTargets(1, &m_rtvHandle, TRUE, NULL);
    pCommandList->RSSetScissorRects(1, &m_scissorRect);
    pCommandList->RSSetViewports(1, &m_viewport);
    pCommandList->SetPipelineState(m_pipelineState.Get());
    pCommandList->SetGraphicsRootSignature(m_rootSignature.Get());
    pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pCommandList->SetGraphicsRootConstantBufferView(0,m_constantBuffer->GetGPUVirtualAddress());
    //pCommandList->ClearDepthStencilView(m_dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1, 0, 0, NULL);
    const float clearColor[] = { 1.0f, 0.0f, 0.0f, 0.0f };
    pCommandList->ClearRenderTargetView(m_rtvHandle, clearColor, 0, NULL);
    for (UINT i = 0; i < numObjects; i++) {
        pCommandList->IASetVertexBuffers(0, 1, &pObjects[i].vertexView);
        //pCommandList->DrawInstanced(pObjects[i].nVertices, 1, 0, 0);
    }
    resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_depthMap.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    pCommandList->ResourceBarrier(1, &resourceBarrier);
}

void LightPerspectiveDepthMap::CreatePSO(ID3D12Device* const device) {
    { //Root Signature
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;

        CD3DX12_ROOT_PARAMETER rootParameters[1] = {};

        rootParameters[0].InitAsConstantBufferView(0);

        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(1, rootParameters, 0, NULL, rootSignatureFlags);
        ComPtr<ID3DBlob> rootSignatureBlob;
        ComPtr<ID3DBlob> errorBlob;
        ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rootSignatureBlob, &errorBlob));
        ThrowIfFailed(device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
    }

    { //PSO
        ComPtr<ID3DBlob> vertexShader;
        ComPtr<ID3DBlob> pixelShader;
        ComPtr<ID3DBlob> geometryShader;
        UINT compileFlags = 0;

        ThrowIfFailed(D3DCompileFromFile(L"LightPerspectiveDepthMap.hlsl", NULL, NULL, "vsMain", "vs_5_1", compileFlags, 0, &vertexShader, NULL));
        ThrowIfFailed(D3DCompileFromFile(L"LightPerspectiveDepthMap.hlsl", NULL, NULL, "psMain", "ps_5_1", compileFlags, 0, &pixelShader, NULL));
        ThrowIfFailed(D3DCompileFromFile(L"LightPerspectiveDepthMap.hlsl", NULL, NULL, "gsMain", "gs_5_1", compileFlags, 0, &geometryShader, NULL));

        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        D3D12_DEPTH_STENCIL_DESC depthDesc = {
            TRUE,
            D3D12_DEPTH_WRITE_MASK_ALL,
            D3D12_COMPARISON_FUNC_LESS,
            FALSE
        };

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {
            m_rootSignature.Get(),
            CD3DX12_SHADER_BYTECODE(vertexShader.Get()),
            CD3DX12_SHADER_BYTECODE(pixelShader.Get()),
            {},
            {},
            CD3DX12_SHADER_BYTECODE(geometryShader.Get()),
            {},
            CD3DX12_BLEND_DESC(D3D12_DEFAULT),
            UINT_MAX,
            CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
            {}, //depthDesc
            { inputElementDescs, _countof(inputElementDescs) },
            D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED,
            D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            1
        };
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;
        //psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        psoDesc.SampleDesc.Count = 1;
        ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
        m_pipelineState->SetName(L"LightPerspectiveDepthMap PSO");
    }
}

void LightPerspectiveDepthMap::CreateCBV(ID3D12Device* const device, D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle) {
    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC resourceDesc = {
        D3D12_RESOURCE_DIMENSION_BUFFER,
        0,
        256, //sizeof(CubeMapCameras) < 256
        1,
        1,
        1,
        DXGI_FORMAT_UNKNOWN,
        {1,0},
        D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
        D3D12_RESOURCE_FLAG_NONE
    };
    device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&m_constantBuffer));

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {m_constantBuffer->GetGPUVirtualAddress(),256};
    device->CreateConstantBufferView(&cbvDesc, cbvHandle);
}

void LightPerspectiveDepthMap::CreateSRV(ID3D12Device* const device, D3D12_CPU_DESCRIPTOR_HANDLE srvHandle) {
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {
        DXGI_FORMAT_R32_FLOAT,
        D3D12_SRV_DIMENSION_TEXTURE2DARRAY,
        D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING
    };

    srvDesc.Texture2DArray = {
        0,
        1,
        0,
        6,
        0,
        0.0f
    };

    device->CreateShaderResourceView(m_depthMap.Get(), &srvDesc, srvHandle);
}

XMMATRIX GetViewMatrix(XMFLOAT3 right, XMFLOAT3 up, XMFLOAT3 direction, XMFLOAT3 position) {
    XMVECTOR r = XMLoadFloat3(&right);
    XMVECTOR u = XMLoadFloat3(&up);
    XMVECTOR d = XMLoadFloat3(&direction);
    XMVECTOR p = XMLoadFloat3(&position);

    d = XMVector3Normalize(d);
    u = XMVector3Normalize(XMVector3Cross(d, r));
    r = XMVector3Cross(u, d);

    XMStoreFloat3(&right, r);
    XMStoreFloat3(&up, u);
    XMStoreFloat3(&direction, d);

    float x = -XMVectorGetX(XMVector3Dot(p, r));
    float y = -XMVectorGetX(XMVector3Dot(p, u));
    float z = -XMVectorGetX(XMVector3Dot(p, d));

    XMFLOAT4X4 view = {
        right.x, up.x, direction.x, 0.0f,
        right.y, up.y, direction.y, 0.0f,
        right.z, up.z, direction.z, 0.0f,
        x, y, z, 1.0f,
    };
    return XMLoadFloat4x4(&view);
}

void LightPerspectiveDepthMap::CreateCameras() {
    XMMATRIX projMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, 1.0f, 0.01f, 100.0f);
    m_cubeMapCameras.xP = XMMatrixTranspose(XMMatrixMultiply(GetViewMatrix({ 1.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f }, { 0.0f,0.0f,1.0f }, {position.x,position.y,position.z}), projMatrix));
    m_cubeMapCameras.xM = XMMatrixTranspose(XMMatrixMultiply(GetViewMatrix({ -1.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f }, { 0.0f,0.0f,-1.0f }, { position.x,position.y,position.z }), projMatrix));

    m_cubeMapCameras.yP = XMMatrixTranspose(XMMatrixMultiply(GetViewMatrix({ 1.0f,0.0f,0.0f }, { 0.0f,0.0f,-1.0f }, { 0.0f,1.0f,0.0f }, { position.x,position.y,position.z }), projMatrix));
    m_cubeMapCameras.yM = XMMatrixTranspose(XMMatrixMultiply(GetViewMatrix({ 1.0f,0.0f,0.0f }, { 0.0f,0.0f,1.0f }, { 0.0f,-1.0f,0.0f }, { position.x,position.y,position.z }), projMatrix));

    m_cubeMapCameras.zP = XMMatrixTranspose(XMMatrixMultiply(GetViewMatrix({ 0.0f,0.0f,-1.0f }, { 0.0f,1.0f,0.0f }, { 1.0f,0.0f,0.0f }, { position.x,position.y,position.z }), projMatrix));
    m_cubeMapCameras.zM = XMMatrixTranspose(XMMatrixMultiply(GetViewMatrix({ 0.0f,0.0f,1.0f }, { 0.0f,1.0f,0.0f }, { -1.0f,0.0f,0.0f }, { position.x,position.y,position.z }), projMatrix));
}

D3D12_GPU_VIRTUAL_ADDRESS LightPerspectiveDepthMap::GetCameras() {
    return m_constantBuffer->GetGPUVirtualAddress();
}