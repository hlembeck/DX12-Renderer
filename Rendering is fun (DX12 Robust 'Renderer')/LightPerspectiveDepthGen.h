#pragma once
#include "Shared.h"

//Generates a layered cube-map for a given point light source. The cube-map stores distance from the light, as well as other surface/material attributes. This is used to generate more realistic specular reflections and refractions. See https://developer.nvidia.com/gpugems/gpugems3/part-iii-rendering/chapter-17-robust-multiple-specular-reflections-and-refractions
class LightPerspectiveDepthMap : private PointLight {

	ComPtr<ID3D12Resource> m_pDepthMaps[6]; //The cube map. Each resource is a layered depth map (see depth peeling).
};