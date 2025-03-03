#pragma once
#include "BaseShader.h"
#include "..\DXFramework\TextureManager.h"

using namespace std;
using namespace DirectX;


class HorizontalGaussianblur : public BaseShader
{

public:

	struct ScreenSizeBufferType
	{
		float screenWidth;
		float screenHeight;
		XMFLOAT2 Padding;
	};

	HorizontalGaussianblur(ID3D11Device* device, HWND hwnd,const wchar_t* PixelShader);
	~HorizontalGaussianblur();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, TextureManager* textureMgr, float width, float height);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);


private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* screenSizeBuffer;


	float R_offset;
	float G_offset;

	float B_Offset;
	float vignetteStrength;
	float vignetteRadius;
	float FilmGrainStrengh;






};

