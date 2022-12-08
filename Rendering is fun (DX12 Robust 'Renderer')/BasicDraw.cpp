#include "BasicDraw.h"

BasicDraw::BasicDraw() {}

void BasicDraw::Load() {
	CreateRootSignature();
	CreatePipelineState();
    CreateLightBuffer();
}

void BasicDraw::LoadObject(XMFLOAT4* vertices, UINT numVertices, XMMATRIX worldMatrix) {
    PlacedObject placedObj = {};

    UINT bufSize = 2*sizeof(XMFLOAT4) * numVertices;

    CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufSize);
    ThrowIfFailed(DXBase::m_device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&placedObj.m_vertices)
    ));

    UINT8* pVertexDataBegin;
    CD3DX12_RANGE readRange(0, 0);
    ThrowIfFailed(placedObj.m_vertices->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
    memcpy(pVertexDataBegin, vertices, bufSize);
    placedObj.m_vertices->Unmap(0, nullptr);

    placedObj.m_worldMatrix = worldMatrix;
    placedObj.m_numVertices = numVertices;
    placedObj.m_vertexBufferView = {placedObj.m_vertices->GetGPUVirtualAddress(), bufSize, 2*sizeof(XMFLOAT4)};
    m_objects.push_back(placedObj);
}

void BasicDraw::Draw(ID3D12GraphicsCommandList* pCommandList, RootConstants rootConstants, PointLight* lights, UINT numLights) {

    {
        UINT bufSize = sizeof(PointLight) * std::min(MAXLIGHTS,numLights);
        UINT8* pData;
        CD3DX12_RANGE readRange(0, 0);
        ThrowIfFailed(m_lightBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pData)));
        memset(pData, 0, bufSize);
        memcpy(pData, lights, bufSize);
        m_lightBuffer->Unmap(0, nullptr);
    }

    pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pCommandList->SetPipelineState(m_pipelineState.Get());
    pCommandList->SetGraphicsRootSignature(m_rootSignature.Get());
    pCommandList->SetGraphicsRoot32BitConstants(0, sizeof(RootConstants) / 4, &rootConstants, 0);
    pCommandList->SetGraphicsRootConstantBufferView(1, m_lightBuffer->GetGPUVirtualAddress());

    for (UINT i = 0; i < m_objects.size(); i++) {
        PlacedObject curr = m_objects[i];
        pCommandList->IASetVertexBuffers(0, 1, &curr.m_vertexBufferView);
        pCommandList->DrawInstanced(curr.m_numVertices,1,0,0);
    }
}

void BasicDraw::CreatePipelineState() {
    ComPtr<ID3DBlob> vertexShader;
    ComPtr<ID3DBlob> pixelShader;
    UINT compileFlags = 0;

    ThrowIfFailed(D3DCompileFromFile(L"BasicDraw.hlsl", NULL, NULL, "vsMain", "vs_5_1", compileFlags, 0, &vertexShader, NULL));
    ThrowIfFailed(D3DCompileFromFile(L"BasicDraw.hlsl", NULL, NULL, "psMain", "ps_5_1", compileFlags, 0, &pixelShader, NULL));
    printf("test\n");

    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {
		m_rootSignature.Get(),
        CD3DX12_SHADER_BYTECODE(vertexShader.Get()),
        CD3DX12_SHADER_BYTECODE(pixelShader.Get()),
        {},
        {},
        {},
        {},
        CD3DX12_BLEND_DESC(D3D12_DEFAULT),
        UINT_MAX,
        CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
        {},
        { inputElementDescs, _countof(inputElementDescs) },
        D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED,
        D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
        1
	};
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;
    ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
}

void BasicDraw::CreateRootSignature() {
    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;

    CD3DX12_ROOT_PARAMETER rootParameters[2] = {};

    //CD3DX12_DESCRIPTOR_RANGE descriptorRange[1] = {};
    //descriptorRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0);
    //rootParameters[0].InitAsDescriptorTable(1, descriptorRange); //SRV for the texture that was rendered to.
    //rootParameters[1].InitAsConstantBufferView(0);

    rootParameters[0].InitAsConstants(sizeof(RootConstants) / 4, 0);
    rootParameters[1].InitAsConstantBufferView(1);

    CD3DX12_STATIC_SAMPLER_DESC staticSamplers[1] = {};

    CD3DX12_STATIC_SAMPLER_DESC texSampler = CD3DX12_STATIC_SAMPLER_DESC(
        1,
        D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        D3D12_TEXTURE_ADDRESS_MODE_WRAP
    );
    texSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    staticSamplers[0] = texSampler;

    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(_countof(rootParameters), rootParameters, 1, staticSamplers, rootSignatureFlags);
    ComPtr<ID3DBlob> rootSignatureBlob;
    ComPtr<ID3DBlob> errorBlob;
    ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rootSignatureBlob, &errorBlob));
    ThrowIfFailed(m_device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
}

void BasicDraw::CreateLightBuffer() {
    UINT bufSize = sizeof(PointLight) * MAXLIGHTS;

    CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufSize);
    ThrowIfFailed(DXBase::m_device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_lightBuffer)
    ));
}