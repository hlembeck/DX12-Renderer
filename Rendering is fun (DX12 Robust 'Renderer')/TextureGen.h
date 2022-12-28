#pragma once

#include "DXInstance.h"

//input data assumed unpadded
HRESULT GetTexture2D(ComPtr<ID3D12Resource>& resource, ID3D12Device* const device, const UINT width, const UINT height, const UINT arrayDepth = 1, const DXGI_FORMAT format = DXGI_FORMAT_R32_FLOAT, const D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, const D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE, const void* pData = NULL, D3D12_CLEAR_VALUE* clearVal = NULL);

HRESULT GetTexture3D(ComPtr<ID3D12Resource>& resource, ID3D12Device* const device, const UINT width, const UINT height, const UINT depth, const DXGI_FORMAT format = DXGI_FORMAT_R32_FLOAT, const D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, const D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE, const void* pData = NULL, D3D12_CLEAR_VALUE* clearVal = NULL);

//template <typename T>
//class RandomTextureGen : private virtual DXBase {
//public:
//	RandomTextureGen(DXGI_FORMAT format = DXGI_FORMAT_R32_FLOAT);
//
//	HRESULT GetTexture2D(ComPtr<ID3D12Resource>& resource, const UINT width, const UINT height, const UINT arrayDepth, const D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, const D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);
//private:
//	const DXGI_FORMAT m_format;
//
//};