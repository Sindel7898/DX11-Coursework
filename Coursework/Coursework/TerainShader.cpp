#include "TerainShader.h"

TerainShader::TerainShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"Terrain_VS.cso", L"Terrain_PS.cso");
}


TerainShader::~TerainShader()
{
	// Release the sampler state.
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

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

	// Release the light constant buffer.
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void TerainShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

	D3D11_BUFFER_DESC ManipulationBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);
	loadHullShader(L"tessellation_hs.cso");
	loadDomainShader(L"tessellation_ds.cso");


	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(NewMatrixData);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	// Setup light buffer
	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType) * 4;
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);



	ManipulationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	ManipulationBufferDesc.ByteWidth = sizeof(MannipulationBufferType);
	ManipulationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ManipulationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ManipulationBufferDesc.MiscFlags = 0;
	ManipulationBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&ManipulationBufferDesc, NULL, &ManipulationBuffer);


	D3D11_BUFFER_DESC CameraBufferDesc;
	CameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	CameraBufferDesc.ByteWidth = sizeof(CameraData);
	CameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	CameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CameraBufferDesc.MiscFlags = 0;
	CameraBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&CameraBufferDesc, NULL, &CameraBuffer);

}


void TerainShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, XMFLOAT3 cameraposition, TextureManager* textureMgr, ID3D11ShaderResourceView* DepthTexture, std::vector<Light*>& Lights, float mHeight, XMFLOAT3 CameraPosition)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	D3D11_MAPPED_SUBRESOURCE lightResource;

	NewMatrixData* dataPtr;
	
	XMMATRIX tworld, tview, tproj;
	XMMATRIX  LightView, LightProj;
	XMMATRIX  LightView2, LightProj2;


	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);
	LightView = XMMatrixTranspose(Lights[0]->getViewMatrix());
	LightProj = XMMatrixTranspose(Lights[0]->getProjectionMatrix());


	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (NewMatrixData*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	dataPtr->lightView = LightView;
	dataPtr->lightProjection = LightProj;

	dataPtr->CameraPosition = cameraposition;
	dataPtr->Padding = 0.0f;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);



	////Additional
	//// Send light data to pixel shader
	LightBufferType* lightPtr;
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &lightResource);
	lightPtr = (LightBufferType*)lightResource.pData;


	for (int i = 0; i < Lights.size(); ++i)
	{
		// Set light data for each light in the scene

		lightPtr[i].Position = Lights[i]->getPosition();
		lightPtr[i].padding = 0.0f;
		lightPtr[i].Direction = Lights[i]->getDirection();;
		lightPtr[i].padding2 = 0.0f;

		lightPtr[i].diffuse = Lights[i]->getDiffuseColour();

		lightPtr[i].LightIntensity = Lights[i]->getLightIntensity();
		lightPtr[i].padding3 = { 0.0f,0.0f,0.0f };

		lightPtr[i].LightType = Lights[i]->GetLightType();
		lightPtr[i].padding4 = { 0.0f,0.0f,0.0f };

		lightPtr[i].ConeParams = Lights[i]->GetConeParams();
		lightPtr[i].padding5 = { 0.0f,0.0f };


		lightPtr[i].specularColour = Lights[i]->getSpecularColour();

		lightPtr[i].specularPower = Lights[i]->getSpecularPower();
		lightPtr[i].padding6 = { 0.0f,0.0f,0.0f };
	}


	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);
	// Map and set the manipulation buffer

	MannipulationBufferType* mannipulationBufferType;
	deviceContext->Map(ManipulationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	mannipulationBufferType = (MannipulationBufferType*)mappedResource.pData;
	mannipulationBufferType->Height = mHeight;
	mannipulationBufferType->Padding = { 0.0f,0.0f,0.0f };


	deviceContext->Unmap(ManipulationBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &ManipulationBuffer);
	
	
	D3D11_MAPPED_SUBRESOURCE SecondMapped;
	// Map and set the camera buffer

	deviceContext->Map(CameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SecondMapped);
	CameraData* data = (CameraData*)SecondMapped.pData;
	data->CameraPosition = CameraPosition;
	data->Paddding = 0.0f;
	deviceContext->Unmap(CameraBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &CameraBuffer);

	ID3D11ShaderResourceView* TerrainTexture;

	// Set shader resources

	TerrainTexture = textureMgr->getTexture(L"Terrain");


	//// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(2, 1, &TerrainTexture);
	deviceContext->PSSetShaderResources(3, 1, &DepthTexture);

	D3D11_SAMPLER_DESC samplerDesc;

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	samplerDesc.MipLODBias = 0.0f; 
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX; 
	renderer->CreateSamplerState(&samplerDesc, &sampleStateShadow);
	deviceContext->PSSetSamplers(3, 1, &sampleStateShadow);


}

void TerainShader::LoadHeightMap(ID3D11Device* Device, ID3D11DeviceContext* context, const wchar_t* FileName)
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


	context->PSSetShaderResources(1, 1, &HeightMapTexture);
	context->PSSetSamplers(1, 1, &HeightMapSampler);
}



