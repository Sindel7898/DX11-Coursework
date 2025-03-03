// Colour shader.h
// Simple shader example.
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class DepthShader : public BaseShader
{

public:
	struct DepthMatrixBuffer
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		float Height;
		XMFLOAT3 Padding;
	};

	struct SecondDepthMatrixBuffer
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};
	DepthShader(ID3D11Device* device, HWND hwnd, const wchar_t* vsFilename);
	~DepthShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projectionz, float Height);
	void SecondShadowShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projectionz);

	void LoadHeightMap(ID3D11Device* Device, ID3D11DeviceContext* context, const wchar_t* FileName);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* SecondmatrixBuffer;

	ID3D11ShaderResourceView* HeightMapTexture;

};
