#pragma once
#include "DXInstance.h"

//Generates terrain from the standard Perlin Noise method, and cannot allow things like caves due to the heightmap property.
//Can use to aid generation of more complex terrains.
class HeightmapTerrain :
	private virtual DXBase,
	public BasicRenderObject
{
public:
	HeightmapTerrain(UINT quality = 2);
	void Load(ID3D12CommandQueue* commandQueue);
	void UpdateTerrain(XMFLOAT4 position, ID3D12CommandQueue* const commandQueue);
	void LinkDevice(ComPtr<ID3D12Device> device);
private:
	void LoadTexture(ID3D12CommandQueue* commandQueue);
	void CreatePipelineState();
	void LoadVertices();
	void CreateRootSignature();
	void Wait(ID3D12CommandQueue* const commandQueue);
	void ExecuteCommandList(ID3D12CommandQueue* const commandQueue);
	void CreateDescriptorHeap();
	UINT m_quality;
	ComPtr<ID3D12Resource> m_randomTexture;
	ComPtr<ID3D12DescriptorHeap> m_srvHeap;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	ComPtr<ID3D12Resource> m_vertices;
	ComPtr<ID3D12PipelineState> m_pipelineState;
	ComPtr<ID3D12RootSignature> m_rootSignature;

	//Fence
	UINT64 m_fenceValue;
	HANDLE m_fenceEvent;
	ComPtr<ID3D12Fence> m_fence;

	XMFLOAT2 m_pos;
};