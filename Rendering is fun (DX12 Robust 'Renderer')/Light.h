#pragma once
#include "DXInstance.h"

constexpr UINT MAXLIGHTS = 5;


class PLTest : 
	private virtual RandomGenerator {
public:
	PLTest(XMFLOAT4 initialPosition = {}, XMFLOAT4 color = {});

	PointLight GetLight();
	void UpdatePosition(float elapsedTime);
private:
	XMFLOAT4 GetPosition();
	XMFLOAT4 m_wPosition;
	XMFLOAT2 m_sPosition;

	XMFLOAT2 m_angularVelocity;
	XMFLOAT4 m_color;
};


class Lights : private virtual DXBase {
protected:
	Lights();

	void Load();
	void OnUpdate(float elapsedTime);
	D3D12_GPU_VIRTUAL_ADDRESS GetView();
private:
	PLTest m_pointLights[MAXLIGHTS];
	ComPtr<ID3D12Resource> m_lightBuffer;
};