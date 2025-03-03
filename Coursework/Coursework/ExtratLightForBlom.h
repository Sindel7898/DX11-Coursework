#pragma once
#include "BaseShader.h"
#include "..\DXFramework\TextureManager.h"

using namespace std;
using namespace DirectX;


class ExtratLightForBlom : public BaseShader
{

public:
	struct BloomData
	{
		float BrightnessThresh;
		XMFLOAT3 Padding;

	};

	ExtratLightForBlom(ID3D11Device* device, HWND hwnd);
	~ExtratLightForBlom();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, TextureManager* textureMgr, float width, float height);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);


private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* BloomDataBuffer;

	float BrightnessThresh = 0;

};

