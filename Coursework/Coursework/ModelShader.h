#pragma once
#include "DXF.h"

using namespace std;
using namespace DirectX;

class ModelShader : public BaseShader
{
private:

	struct NewMatrixData
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMFLOAT3 CameraLocation;
		float Padding;
		XMMATRIX lightProjection;
		XMMATRIX lightView;

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




public:
	ModelShader(ID3D11Device* device, HWND hwnd, const wchar_t* psFilenam);
	~ModelShader();

	virtual void initShader(const wchar_t* vsFilename, const wchar_t* psFilename) override;


	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, XMFLOAT3 cameraposition, TextureManager* textureMg, std::vector<Light*>& Lights, ID3D11ShaderResourceView* DepthTexture, const wchar_t* albedoTexture, const wchar_t* HeightTexture, const wchar_t* Metal, const wchar_t* Roughnes);

	void BASICShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix);


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

