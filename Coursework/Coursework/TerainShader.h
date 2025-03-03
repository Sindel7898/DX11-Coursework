#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class TerainShader : public BaseShader
{
private:

	struct NewMatrixData
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
		XMMATRIX lightView2;
		XMMATRIX lightProjection2;
		XMFLOAT3 CameraPosition;
		float Padding;
	};



	struct LightBufferType
	{
		XMFLOAT3 Position;
		float padding;
		XMFLOAT3 Direction;
		float padding2;

		XMFLOAT4 diffuse;

		float LightIntensity;
		XMFLOAT3 padding3;

		float LightType;
		XMFLOAT3 padding4;

		XMFLOAT2 ConeParams;
		XMFLOAT2 padding5;

		XMFLOAT4 specularColour;

		float specularPower;
		XMFLOAT3 padding6;
	};



	struct MannipulationBufferType
	{
		float Height;
		XMFLOAT3 Padding;
	

	};


	struct HeightVertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
	};

	struct CameraData
	{
		XMFLOAT3 CameraPosition;
		float Paddding;
	};


public:
	TerainShader(ID3D11Device* device, HWND hwnd);
	~TerainShader();

	virtual void initShader(const wchar_t* vsFilename, const wchar_t* psFilename) override;


	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, XMFLOAT3 cameraposition, TextureManager* textureMgr, ID3D11ShaderResourceView* DepthTexture, std::vector<Light*>& Lights, float mHeight, XMFLOAT3 CameraPosition);

	void LoadHeightMap(ID3D11Device* Device, ID3D11DeviceContext* context, const wchar_t* FileName);

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

