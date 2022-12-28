#pragma once
#include "DXInstance.h"
#include "LightPerspectiveDepthGen.h"

constexpr UINT MAXLIGHTS = 1;

class PLTest : 
	private virtual RandomGenerator {
public:
	PLTest(XMFLOAT4 initialPosition = {}, XMFLOAT4 color = {});

	PointLight GetLight();
	void UpdatePosition(float elapsedTime, XMFLOAT4 pos);
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
	void OnUpdate(float elapsedTime, XMFLOAT4 pos);
	D3D12_GPU_VIRTUAL_ADDRESS GetView();
private:
	PLTest m_pointLights[MAXLIGHTS];
	ComPtr<ID3D12Resource> m_lightBuffer;
};

class RobustLight : private LightPerspectiveDepthMap {
public:
	RobustLight(XMFLOAT4 pos = {}, XMFLOAT4 color = {});
	void UpdatePosition(XMFLOAT4 pos);
};