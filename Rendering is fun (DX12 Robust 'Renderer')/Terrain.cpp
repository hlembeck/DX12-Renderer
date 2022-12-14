#include "Terrain.h"

Terrain::Terrain() : m_terrain(4) {}

void Terrain::Load(ID3D12CommandQueue* commandQueue) {
    SimpleVertex vertices[6] = {
        {{-5.0f,-5.0f,5.0f,1.0f}, {0.0f,0.0f,-1.0f,0.0f} },
        {{-5.0f,5.0f,5.0f,1.0f}, {0.0f,0.0f,-1.0f,0.0f}},
        {{5.0f,5.0f,5.0f,1.0f}, {0.0f,0.0f,-1.0f,0.0f}},

        {{-5.0f,-5.0f,5.0f,1.0f}, {0.0f,0.0f,-1.0f,0.0f}},
        {{5.0f,5.0f,5.0f,1.0f}, {0.0f,0.0f,-1.0f,0.0f}},
        {{5.0f,-5.0f,5.0f,1.0f}, {0.0f,0.0f,-1.0f,0.0f}},
    };
    m_mesh.Load(m_device.Get(), vertices, 6, XMMatrixIdentity());
    m_terrain.LinkDevice(m_device);
    m_terrain.Load(commandQueue);
    m_terrain.UpdateTerrain({0.0f,0.0f,0.0f,0.0f}, commandQueue);
}

void Terrain::Update(XMFLOAT4 position, ID3D12CommandQueue* commandQueue) {
    //m_terrain.UpdateTerrain(position, commandQueue);
}

BasicRenderObject Terrain::GetRenderObject() {
    return m_terrain;
}