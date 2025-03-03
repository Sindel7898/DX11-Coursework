// depth shader.cpp
#include "depthshader.h"

DepthShader::DepthShader(ID3D11Device* device, HWND hwnd, const wchar_t* vsFilename) : BaseShader(device, hwnd)
{
	initShader(vsFilename, L"depth_ps.cso");
}

DepthShader::~DepthShader()
{
	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void DepthShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC SecondmatrixBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	ZeroMemory(&matrixBufferDesc, sizeof(matrixBufferDesc));

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(DepthMatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);


	ZeroMemory(&SecondmatrixBufferDesc, sizeof(SecondmatrixBufferDesc));

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	SecondmatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	SecondmatrixBufferDesc.ByteWidth = sizeof(SecondDepthMatrixBuffer);
	SecondmatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	SecondmatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	SecondmatrixBufferDesc.MiscFlags = 0;
	SecondmatrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);
}

void DepthShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, float Height)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	DepthMatrixBuffer* dataPtr;
	
	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (DepthMatrixBuffer*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	dataPtr->Height = Height;
	dataPtr->Padding = { 0.0f,0.0f ,0.0f };

	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);
}


void DepthShader::SecondShadowShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	SecondDepthMatrixBuffer* dataPtr;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (SecondDepthMatrixBuffer*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);
}




void DepthShader::LoadHeightMap(ID3D11Device* Device, ID3D11DeviceContext* context, const wchar_t* FileName)
{
	CreateWICTextureFromFile(Device, context, FileName, nullptr, &HeightMapTexture);

	D3D11_SAMPLER_DESC HeightMapSamplerDesc = {};
	HeightMapSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // Linear filtering
	HeightMapSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;    // Wrap texture addressing mode for U coordinate
	HeightMapSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;    // Wrap texture addressing mode for V coordinate
	HeightMapSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;    // Wrap texture addressing mode for W coordinate
	HeightMapSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;  // No comparison function
	HeightMapSamplerDesc.MinLOD = 0;                               // No minimum level-of-detail
	HeightMapSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11SamplerState* HeightMapSampler;

	Device->CreateSamplerState(&HeightMapSamplerDesc, &HeightMapSampler);


	context->VSSetShaderResources(0, 1, &HeightMapTexture);
	context->VSSetSamplers(0, 1, &HeightMapSampler);

}