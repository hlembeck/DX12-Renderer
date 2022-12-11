#include "Terrain.h"

Terrain::Terrain() {}

void Terrain::Load() {
    SimpleVertex vertices[6] = {
        {{-5.0f,-5.0f,3.0f,1.0f}, {0.0f,0.0f,-1.0f,0.0f} },
        {{-5.0f,5.0f,3.0f,1.0f}, {0.0f,0.0f,-1.0f,0.0f}},
        {{5.0f,5.0f,3.0f,1.0f}, {0.0f,0.0f,-1.0f,0.0f}},

        {{-5.0f,-5.0f,3.0f,1.0f}, {0.0f,0.0f,-1.0f,0.0f}},
        {{5.0f,5.0f,3.0f,1.0f}, {0.0f,0.0f,-1.0f,0.0f}},
        {{5.0f,-5.0f,3.0f,1.0f}, {0.0f,0.0f,-1.0f,0.0f}},
    };
    m_mesh.Load(m_device.Get(), vertices, 6, XMMatrixIdentity());
}

BasicRenderObject* Terrain::GetRenderObject() {
    return &m_mesh;
}