#include "TextureGen.h"

HRESULT GetTexture2D(ComPtr<ID3D12Resource>& resource, ID3D12Device* const device, const UINT width, const UINT height, const UINT arrayDepth, const DXGI_FORMAT format, const D3D12_RESOURCE_STATES initialState, const D3D12_RESOURCE_FLAGS flags, const void* pData, D3D12_CLEAR_VALUE* clearVal) {
	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC resourceDesc = {
		D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		0,
		width,height,arrayDepth,
		1,
		format,
		{1,0},
		D3D12_TEXTURE_LAYOUT_UNKNOWN,
		flags
	};

	return device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, initialState, clearVal, IID_PPV_ARGS(&resource));;
}

HRESULT GetTexture3D(ComPtr<ID3D12Resource>& resource, ID3D12Device* const device, const UINT width, const UINT height, const UINT depth, const DXGI_FORMAT format, const D3D12_RESOURCE_STATES initialState, const D3D12_RESOURCE_FLAGS flags, const void* pData, D3D12_CLEAR_VALUE* clearVal) {
	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC resourceDesc = {
		D3D12_RESOURCE_DIMENSION_TEXTURE3D,
		0,
		width,height,depth,
		1,
		format,
		{1,0},
		D3D12_TEXTURE_LAYOUT_UNKNOWN,
		flags
	};

	return device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, initialState, clearVal, IID_PPV_ARGS(&resource));
}

//RandomTextureGen::RandomTextureGen(DXGI_FORMAT format) : m_format(format) {}
//
//HRESULT RandomTextureGen::GetTexture2D(ComPtr<ID3D12Resource>& resource, const UINT width, const UINT height, const UINT arrayDepth, const D3D12_RESOURCE_STATES initialState, const D3D12_RESOURCE_FLAGS flags) {
//	HRESULT ret = GetTexture2D(resource, width, height, arrayDepth, m_format, initialState, flags);
//}