#include "Light.h"

PLTest::PLTest(XMFLOAT4 initialPosition, XMFLOAT4 color) : m_wPosition(initialPosition), m_color(color) {
    m_color = { (RandomGenerator::Get() + 1.0f) * 0.5f,(RandomGenerator::Get() + 1.0f) * 0.5f,(RandomGenerator::Get()+1.0f)*0.5f,0.0f };
	m_sPosition = { RandomGenerator::Get(),RandomGenerator::Get() };
	m_angularVelocity = { RandomGenerator::Get(),RandomGenerator::Get() };
}

PointLight PLTest::GetLight() {
	return { GetPosition(),m_color };
}

XMFLOAT4 PLTest::GetPosition() {
	return {3.0f*sin(m_sPosition.x)*cos(m_sPosition.y)+ m_wPosition.x,3.0f*sin(m_sPosition.x)*sin(m_sPosition.y)+m_wPosition.y,3.0f*cos(m_sPosition.y)+m_wPosition.z,1.0f};
}

void PLTest::UpdatePosition(float elapsedTime) {
	m_sPosition.x += m_angularVelocity.x*elapsedTime;
	m_sPosition.y += m_angularVelocity.y*elapsedTime;
	m_angularVelocity.x += RandomGenerator::Get();
	m_angularVelocity.y += RandomGenerator::Get();
}

Lights::Lights() : m_pointLights() {}

void Lights::Load() {
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

void Lights::OnUpdate(float elapsedTime) {

    PointLight lights[MAXLIGHTS] = {};

    for (UINT i = 0; i < MAXLIGHTS; i++) {
        lights[i] = m_pointLights[i].GetLight();
    }

    UINT bufSize = sizeof(PointLight) * MAXLIGHTS;
    UINT8* pData;
    CD3DX12_RANGE readRange(0, 0);
    ThrowIfFailed(m_lightBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pData)));
    memset(pData, 0, bufSize);
    memcpy(pData, lights, bufSize);
    m_lightBuffer->Unmap(0, nullptr);

    for (UINT i = 0; i < MAXLIGHTS; i++) {
        m_pointLights[i].UpdatePosition(elapsedTime);
    }
}

D3D12_GPU_VIRTUAL_ADDRESS Lights::GetView() { return m_lightBuffer->GetGPUVirtualAddress(); }