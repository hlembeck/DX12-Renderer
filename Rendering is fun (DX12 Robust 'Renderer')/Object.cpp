#include "Object.h"

SimpleMesh::SimpleMesh() {}

void SimpleMesh::Load(ID3D12Device* device, SimpleVertex* vertices, UINT numVertices, XMMATRIX worldMatrix) {
    UINT bufSize = sizeof(SimpleVertex) * numVertices;

    CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufSize);
    ThrowIfFailed(device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_vertexBuffer)
    ));

    UINT8* pVertexDataBegin;
    CD3DX12_RANGE readRange(0, 0);
    ThrowIfFailed(m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
    memcpy(pVertexDataBegin, vertices, bufSize);
    m_vertexBuffer->Unmap(0, nullptr);

    vertexView = { m_vertexBuffer->GetGPUVirtualAddress(), bufSize, sizeof(SimpleVertex) };
    nVertices = numVertices;
    m_worldMatrix = worldMatrix;
}