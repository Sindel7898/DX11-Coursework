#pragma once
#include "BaseMesh.h"

using namespace DirectX;

class TesselationPlane : public BaseMesh
{

public:
	TesselationPlane(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int lresolution);
	~TesselationPlane();
	void initBuffers(ID3D11Device* device);
	void sendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top = D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST) override;
	float resolution;
};

