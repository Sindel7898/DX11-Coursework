#pragma once
#include "BaseShader.h"
#include "..\DXFramework\TextureManager.h"

using namespace std;
using namespace DirectX;

class CameraStyleShader : public BaseShader
{
public:
	struct ScreenSizeBufferType
	{
		float screenWidth;
		float screenHeight;
		float R_offset;
		float G_offset;

		float B_Offset;
		float vignetteStrength;
		float vignetteRadius;
		float FilmGrainStrengh;

		float BloomStrengh;
		XMFLOAT3 Padding;


	};

	CameraStyleShader(ID3D11Device* device, HWND hwnd);
	~CameraStyleShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* LightTexture, ID3D11ShaderResourceView* BloomBlur, TextureManager* textureMgr,float width, float height);

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
	float BloomStrengh;


};

