#include "Terrain.h"

Terrain::Terrain() : m_terrain(5) {}

void Terrain::Load() {
    SimpleVertex vertices[24] = {
        {{-1.0f,-1.0f,1.0f,1.0f}, {0.0f,0.0f,-1.0f,0.0f} },
        {{-1.0f,1.0f,1.0f,1.0f}, {0.0f,0.0f,-1.0f,0.0f}},
        {{1.0f,1.0f,1.0f,1.0f}, {0.0f,0.0f,-1.0f,0.0f}},
        {{-1.0f,-1.0f,1.0f,1.0f}, {0.0f,0.0f,-1.0f,0.0f}},
        {{1.0f,1.0f,1.0f,1.0f}, {0.0f,0.0f,-1.0f,0.0f}},
        {{1.0f,-1.0f,1.0f,1.0f}, {0.0f,0.0f,-1.0f,0.0f}},

        {{-1.0f,-1.0f,-1.0f,1.0f}, {0.0f,0.0f,1.0f,0.0f}},
        {{1.0f,1.0f,-1.0f,1.0f}, {0.0f,0.0f,1.0f,0.0f}},
        {{-1.0f,1.0f,-1.0f,1.0f}, {0.0f,0.0f,1.0f,0.0f}},
        {{-1.0f,-1.0f,-1.0f,1.0f}, {0.0f,0.0f,1.0f,0.0f}},
        {{1.0f,-1.0f,-1.0f,1.0f}, {0.0f,0.0f,1.0f,0.0f}},
        {{1.0f,1.0f,-1.0f,1.0f}, {0.0f,0.0f,1.0f,0.0f}},

        {{-1.0f,-1.0f,-1.0f,1.0f},{1.0f,0.0f,0.0f,0.0f}},
        {{-1.0f,1.0f,-1.0f,1.0f},{1.0f,0.0f,0.0f,0.0f}},
        {{-1.0f,1.0f,1.0f,1.0f},{1.0f,0.0f,0.0f,0.0f}},
        {{-1.0f,-1.0f,-1.0f,1.0f},{1.0f,0.0f,0.0f,0.0f}},
        {{-1.0f,1.0f,1.0f,1.0f},{1.0f,0.0f,0.0f,0.0f}},
        {{-1.0f,-1.0f,1.0f,1.0f},{1.0f,0.0f,0.0f,0.0f}},

        {{1.0f,-1.0f,1.0f,1.0f},{-1.0f,0.0f,0.0f,0.0f}},
        {{1.0f,1.0f,1.0f,1.0f},{-1.0f,0.0f,0.0f,0.0f}},
        {{1.0f,1.0f,-1.0f,1.0f},{-1.0f,0.0f,0.0f,0.0f}},
        {{1.0f,-1.0f,1.0f,1.0f},{-1.0f,0.0f,0.0f,0.0f}},
        {{1.0f,1.0f,-1.0f,1.0f},{-1.0f,0.0f,0.0f,0.0f}},
        {{1.0f,-1.0f,-1.0f,1.0f},{-1.0f,0.0f,0.0f,0.0f}},
    };
    m_mesh.Load(m_device.Get(), vertices, 24, XMMatrixIdentity());
    m_terrain.LinkDevice(m_device, m_commandQueue);
    m_terrain.Load();
    m_terrain.UpdateTerrain({0.0f,0.0f,0.0f,0.0f});
}

void Terrain::Update(XMFLOAT4 position) {
    m_terrain.UpdateTerrain(position);
}

BasicRenderObject Terrain::GetRenderObject() {
    //return m_terrain;
    return m_mesh;
}