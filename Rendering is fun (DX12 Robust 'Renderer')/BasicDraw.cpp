#include "BasicDraw.h"

BasicDraw::BasicDraw() {}

void BasicDraw::Load() {
	CreateRootSignature();
	CreatePipelineState();
}

void BasicDraw::Draw(ID3D12GraphicsCommandList* pCommandList, BasicRenderObject* pObjects, UINT numObjects, D3D12_GPU_VIRTUAL_ADDRESS lightBufferView) {
    pCommandList->SetPipelineState(m_pipelineState.Get());
    pCommandList->SetGraphicsRootSignature(m_rootSignature.Get());
    pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pCommandList->SetGraphicsRoot32BitConstants(0, sizeof(RootConstants) / 4, &m_rootConstants, 0);
    pCommandList->SetGraphicsRootConstantBufferView(1, lightBufferView);

    for (UINT i = 0; i < numObjects; i++) {
        pCommandList->IASetVertexBuffers(0, 1, &pObjects[i].vertexView);
        pCommandList->DrawInstanced(pObjects[i].nVertices,1,0,0);
    }
}

void BasicDraw::DrawWithShadow(ID3D12GraphicsCommandList* pCommandList, BasicRenderObject* pObjects, UINT numObjects, D3D12_GPU_DESCRIPTOR_HANDLE depthCubeMap, D3D12_GPU_VIRTUAL_ADDRESS cameraView, PointLight pl) {
    pCommandList->SetPipelineState(m_robustLightPipelineState.Get());
    pCommandList->SetGraphicsRootSignature(m_rootSignature.Get());
    pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pCommandList->SetGraphicsRoot32BitConstants(0, sizeof(RootConstants) / 4, &m_rootConstants, 0);
    pCommandList->SetGraphicsRoot32BitConstants(0, 8, &pl, sizeof(RootConstants) / 4);
    pCommandList->SetGraphicsRootDescriptorTable(2, depthCubeMap);
    pCommandList->SetGraphicsRootConstantBufferView(1, cameraView);

    for (UINT i = 0; i < numObjects; i++) {
        pCommandList->IASetVertexBuffers(0, 1, &pObjects[i].vertexView);
        pCommandList->DrawInstanced(pObjects[i].nVertices, 1, 0, 0);
    }
}

void BasicDraw::CreatePipelineState() {
    ComPtr<ID3DBlob> vertexShader;
    ComPtr<ID3DBlob> pixelShader;
    UINT compileFlags = 0;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
    D3D12_DEPTH_STENCIL_DESC depthDesc;
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    {
        ThrowIfFailed(D3DCompileFromFile(L"BasicDraw.hlsl", NULL, NULL, "vsMain", "vs_5_1", compileFlags, 0, &vertexShader, NULL));
        ThrowIfFailed(D3DCompileFromFile(L"BasicDraw.hlsl", NULL, NULL, "psMain", "ps_5_1", compileFlags, 0, &pixelShader, NULL));

        depthDesc = {
            TRUE,
            D3D12_DEPTH_WRITE_MASK_ALL,
            D3D12_COMPARISON_FUNC_LESS,
            FALSE
        };

        psoDesc = {
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
            depthDesc,
            { inputElementDescs, _countof(inputElementDescs) },
            D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED,
            D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            1
        };
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        psoDesc.SampleDesc.Count = 1;
        ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
        m_pipelineState->SetName(L"Basic Draw PSO");
    }

    {
        ThrowIfFailed(D3DCompileFromFile(L"RobustLightDraw.hlsl", NULL, NULL, "vsMain", "vs_5_1", compileFlags, 0, &vertexShader, NULL));
        ThrowIfFailed(D3DCompileFromFile(L"RobustLightDraw.hlsl", NULL, NULL, "psMain", "ps_5_1", compileFlags, 0, &pixelShader, NULL));

        depthDesc = {
            TRUE,
            D3D12_DEPTH_WRITE_MASK_ALL,
            D3D12_COMPARISON_FUNC_LESS,
            FALSE
        };

        psoDesc = {
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
            depthDesc,
            { inputElementDescs, _countof(inputElementDescs) },
            D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED,
            D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            1
        };
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        psoDesc.SampleDesc.Count = 1;
        ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_robustLightPipelineState)));
        m_robustLightPipelineState->SetName(L"Robust Light Draw PSO");
    }
}

void BasicDraw::CreateRootSignature() {
    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;

    CD3DX12_ROOT_PARAMETER rootParameters[3] = {};

    CD3DX12_DESCRIPTOR_RANGE descriptorRange[1] = {};
    descriptorRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
    //descriptorRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

    rootParameters[0].InitAsConstants(sizeof(RootConstants) / 4 + 8, 0);
    rootParameters[1].InitAsConstantBufferView(1);
    rootParameters[2].InitAsDescriptorTable(1, descriptorRange);

    CD3DX12_STATIC_SAMPLER_DESC staticSamplers[1] = {};

    CD3DX12_STATIC_SAMPLER_DESC texSampler = CD3DX12_STATIC_SAMPLER_DESC(
        0,
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