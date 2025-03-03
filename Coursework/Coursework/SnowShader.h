#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class SnowShader : public BaseShader
{
private:

	struct NewMatrixData
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	
	};


public:
	SnowShader(ID3D11Device* device, HWND hwnd);
	~SnowShader();

	virtual void initShader(const wchar_t* vsFilename, const wchar_t* psFilename) override;


	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix);

private:
	ID3D11Buffer* CameraBuffer;

private:
	ID3D11SamplerState* sampleStateShadow;
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* ManipulationBuffer;
	ID3D11ShaderResourceView* HeightMapTexture;
	XMFLOAT3 LightPosition = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 LightDirection = { 0.0f, -1.0f, 0.0f };
	XMFLOAT3 LightColor = { 1.0f, 1.0f, 1.0f };
	float lightIntensity = 3;
	float ellappsedtime;
};

