#include "ModelShader.h"


ModelShader::ModelShader(ID3D11Device* device, HWND hwnd, const wchar_t* psFilenam) : BaseShader(device, hwnd)
{
	initShader(L"Model_VS.cso", psFilenam);
}


ModelShader::~ModelShader()
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

void ModelShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

	// Load (+ compile) shader files
	//loadModelVertexShader(vsFilename);
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(NewMatrixData);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// Create a texture sampler state description.
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



}


void ModelShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, XMFLOAT3 cameraposition, TextureManager* textureMgr, std::vector<Light*>& Lights, ID3D11ShaderResourceView* DepthTexture,
	                                  const wchar_t* albedoTexture, const wchar_t* HeightTexture, const wchar_t* Metal, const wchar_t* Roughnes)
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
	LightView = XMMatrixTranspose(Lights[2]->getViewMatrix());
	LightProj = XMMatrixTranspose(Lights[2]->getProjectionMatrix());


	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (NewMatrixData*)mappedResource.pData;
	dataPtr->world = tworld;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	dataPtr->CameraLocation = cameraposition;
	dataPtr->Padding = 0.0f;
	dataPtr->lightProjection = LightProj;
	dataPtr->lightView = LightView;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &matrixBuffer);



	////Additional
	//// Send light data to pixel shader
	LightBufferType* lightPtr;
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &lightResource);
	lightPtr = (LightBufferType*)lightResource.pData;


	for (int i = 0; i < Lights.size(); ++i)
	{
		// positional 
		lightPtr[i].Position = Lights[i]->getPosition();
		lightPtr[i].padding = 0.0f;
		lightPtr[i].Direction = Lights[i]->getDirection();;
		lightPtr[i].padding2 = 0.0f;

		//Color
		lightPtr[i].diffuse = Lights[i]->getDiffuseColour();

		//Intensity
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


	ID3D11ShaderResourceView* BaseTexture;
	BaseTexture = textureMgr->getTexture(albedoTexture);

	ID3D11ShaderResourceView*  HeightMap;
	HeightMap = textureMgr->getTexture(HeightTexture);

	ID3D11ShaderResourceView* MetalMap;
	MetalMap = textureMgr->getTexture(Metal);


	ID3D11ShaderResourceView* RoughnessMap;
	RoughnessMap = textureMgr->getTexture(Roughnes);


	//// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(10, 1, &BaseTexture);
	deviceContext->PSSetShaderResources(11, 1, &HeightMap);
	deviceContext->PSSetShaderResources(12, 1, &MetalMap);
	deviceContext->PSSetShaderResources(13, 1, &RoughnessMap);
	deviceContext->PSSetShaderResources(14, 1, &DepthTexture);
	

	deviceContext->VSSetShaderResources(11, 1, &HeightMap);

	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->VSSetSamplers(0, 1, &sampleState);


}


void ModelShader::BASICShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix){
	
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

	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (NewMatrixData*)mappedResource.pData;
	dataPtr->world = tworld;
	dataPtr->view = tview;
	dataPtr->projection = tproj;

	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &matrixBuffer);
}
