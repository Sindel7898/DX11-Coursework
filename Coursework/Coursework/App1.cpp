// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{

}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);
	
	//Load Textures
	textureMgr->loadTexture(L"Terrain", L"res/LandTex2.png");
	textureMgr->loadTexture(L"Brick_Base", L"res/alley-brick-wall_albedo.png");
	textureMgr->loadTexture(L"Brick_Height", L"res/alley-brick-wall_height.png");
	textureMgr->loadTexture(L"Brick_Metal", L"res/alley-brick-wall_metallic.png");
	textureMgr->loadTexture(L"Brick_Rough", L"res/alley-brick-wall_roughness.png");

	textureMgr->loadTexture(L"Wood_Base", L"res/ancient-sewer-stonework_albedo.png");
	textureMgr->loadTexture(L"Wood_Height", L"res/ancient-sewer-stonework_height.png");
	textureMgr->loadTexture(L"Wood_Metal", L"res/ancient-sewer-stonework_metallic.png");
	textureMgr->loadTexture(L"Wood_Rough", L"res/ancient-sewer-stonework_roughness.png");
	//////////////////////////////////////////////////////////////////////////////////////


	// Initalise scene variables.
	mesh = new TesselationPlane(renderer->getDevice(), renderer->getDeviceContext(), 100);
	Sphere = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext(), 10);
	Sphere2 = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext(), 10);
	Sphere3 = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext(), 10);
	BrickSphere = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext(), 10);
	Floor = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext(), 100);
	Cube = new CubeMesh(renderer->getDevice(), renderer->getDeviceContext(), 10);


	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);	
	SnowPoints = new PointList(renderer->getDevice(), renderer->getDeviceContext());
	depthShader = new DepthShader(renderer->getDevice(), hwnd, L"depth_vs.cso");
	SecondDepthShader = new DepthShader(renderer->getDevice(), hwnd, L"SecondShadowDepth_VS.cso");

	// Initalise Shaders.
	terrainshader = new TerainShader(renderer->getDevice(), hwnd);
	CameraStyleShader1 = new CameraStyleShader(renderer->getDevice(), hwnd);
	extractlightsforBloomShader = new ExtratLightForBlom(renderer->getDevice(), hwnd);
	HorizontalGaussianblurShader = new HorizontalGaussianblur(renderer->getDevice(), hwnd, L"HorizontalBlur_PS.cso");
	VerticalGaussianblurShader = new HorizontalGaussianblur(renderer->getDevice(), hwnd, L"VerticalBlur_PS.cso");
	Snowshader = new  SnowShader(renderer->getDevice(), hwnd);
	modelShader = new ModelShader(renderer->getDevice(), hwnd, L"Model_PS.cso");
	LightShader =  new ModelShader(renderer->getDevice(), hwnd, L"OUTPUTLIGHT.cso");
	textureshader = new TextureShader(renderer->getDevice(), hwnd);

	//load Height maps shader
	terrainshader->LoadHeightMap(renderer->getDevice(), renderer->getDeviceContext(), L"res/Height4.png");
	depthShader->LoadHeightMap(renderer->getDevice(), renderer->getDeviceContext(), L"res/Height4.png");

	//initialise render textures
	LightTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	ExtractLightsforBloomTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	
	HorizontalGaussianblurTexture = new RenderTexture(renderer->getDevice(), screenWidth , screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	VerticalGaussianblurTexture = new RenderTexture(renderer->getDevice(), screenWidth , screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	SecondHorizontalGaussianblurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	SecondVerticalGaussianblurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	ThirdHorizontalGaussianblurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	ThirdVerticalGaussianblurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);


	ForthHorizontalGaussianblurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	ForthVerticalGaussianblurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);


	fifthHorizontalGaussianblurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	fifthVerticalGaussianblurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	


	SixthHorizontalGaussianblurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	SixthVerticalGaussianblurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	
	CameraEffectsTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	// Configure Lights 
	light = std::make_unique<Light>();
	light->setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	light->setDiffuseColour(0.033f, 0.005f, 0.142f, 1.0f);
	light->setDirection(-1.0f, 0.176f, -1.0f);
	light->Newposition = { -88.0f, -1.0f, -168.0f };
	light->generateOrthoMatrix(1000, 1000, 1.0f, 100.0f);
	light->generateProjectionMatrix(1.0f, 100);
	light->SetLightIntensity(4.54);
	light->SetLightType(2);
	light->SetConeParams(30, 60);



	light2 = std::make_unique<Light>();
	light2->setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	light2->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	light2->setDirection(1.0f, -1.0f, -1.0f);
	light2->generateOrthoMatrix(1000, 1000, 1.0f, 100.0f);
	light2->generateProjectionMatrix(1.0f, 100);
	light2->SetLightType(2);
	light2->SetConeParams(30, 60);



	light3 = std::make_unique<Light>();
	light3->setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	light3->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	light3->Newposition = { -13.0f, 72.0f, -5.0f };

	light3->setDirection(1.0f, -1.0f, 1.0f);
	light3->generateOrthoMatrix(1920, 1920, 1.0f, 100.0f);
	light3->generateProjectionMatrix(1.0f, 100);
	light3->SetLightType(0);
	light3->SetConeParams(30, 60);
	light3->SetLightIntensity(100);
	light3->SetSpecularPower(100);


	light4 = std::make_unique<Light>();
	light4->setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	light4->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	light4->Newposition = { 8.0f, 72.0f, 0.0f };
	light4->setDirection(1.0f, -1.0f, -1.0f);
	light4->generateOrthoMatrix(1920, 1920, 1.0f, 100.0f);
	light4->generateProjectionMatrix(1.0f, 100);
	light4->SetLightType(0);
	light4->SetConeParams(30, 60);
	light4->SetLightIntensity(100);
	light4->SetSpecularPower(100);

	light5 = std::make_unique<Light>();
	light5->setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	light5->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	light5->Newposition = {8.0, 56.0f, -47.0f };
	light5->setDirection(0.0f, -1.0f, 0.0f);
	light5->SetLightType(1);
	light5->SetConeParams(30, 60);
	light5->SetLightIntensity(100);
	light5->SetSpecularPower(100);


	LightsVector.push_back(light.get());
	LightsVector.push_back(light2.get());
	

	ModelLightsVector.push_back(light3.get());
	ModelLightsVector.push_back(light4.get());
	ModelLightsVector.push_back(light5.get());
	//////////////////////////////////////////////////////////////////////////////////////////////

	camera->setPosition(0, 100, 0);
	camera->setRotation(20, 40, 0);

	//Initialise Shadow maps
	shadowMap = std::make_unique<ShadowMap>(renderer->getDevice(), 5840, 5840);
	SecondshadowMap = std::make_unique<ShadowMap>(renderer->getDevice(), 1920, 1080);

	ShadowMapVectors.push_back(shadowMap.get());
	ShadowMapVectors.push_back(SecondshadowMap.get());

}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	
	if (mesh)
	{
		delete mesh;
		mesh = 0;
	}
}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	
	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{
	// GUI
	ImGui::Begin("Light Controls");
	ImGui::SliderFloat("Terrain Height", (&TerrainHeight), -30.0f, 400.0f);
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	ImGui::End();


	ImGui::Begin("Model Controls");
	ImGui::SliderFloat3("Model Location", (&ModelLocation.x), -20.0f, 100.0f);
	ImGui::SliderFloat3("Model Scale", (&ModelScale.x), 1.0f, 10);
	ImGui::End();


	ImGui::Begin("Floor Controls");
	ImGui::SliderFloat3("Floor Location", (&FloorLocation.x), -200.0f, 100.0f);
	ImGui::End();

	ImGui::Begin("Cube Controls");
	ImGui::SliderFloat3("Floor Location", (&CubeLocation.x), -200.0f, 300.0f);
	ImGui::SliderFloat3("Floor scale", (&CubeScale.x), 0.05, 5.0f);

	ImGui::End();
	/////////////////////////////////////////////////////////////////////////////

	//basic physics for snow 
	initParticlePosX += particleVelocityX;
	initParticlePosY -= particleVelocityY;

	//Reset for snow positon 
	if (initParticlePosY < -8)
	{
		initParticlePosX = 0;
		initParticlePosY = 0;
	}


	if (wireframeToggle)
	{
		LightPass();
	}
	else {
		
		depthPass();
		SeconddepthPass();
		
		LightPass();

		PrepforBloom();
		//Blur passes for bloom 
		BlurPass(ExtractLightsforBloomTexture, HorizontalGaussianblurTexture, VerticalGaussianblurTexture);
		BlurPass(VerticalGaussianblurTexture, SecondHorizontalGaussianblurTexture, SecondVerticalGaussianblurTexture);
		BlurPass(SecondVerticalGaussianblurTexture, ThirdHorizontalGaussianblurTexture, ThirdVerticalGaussianblurTexture);
		BlurPass(ThirdVerticalGaussianblurTexture, ForthHorizontalGaussianblurTexture, ForthVerticalGaussianblurTexture);
		BlurPass(ForthVerticalGaussianblurTexture, fifthHorizontalGaussianblurTexture, fifthVerticalGaussianblurTexture);
		BlurPass(fifthVerticalGaussianblurTexture, SixthHorizontalGaussianblurTexture, SixthVerticalGaussianblurTexture);
		//camera style post processing effects 
		PostProcesses();
		LastPass();
	}
   

	gui();

	// Present the rendered scene to the screen.
	renderer->endScene();

	return true;
}

void App1::depthPass()
{
	// Set the render target to be the render to texture.
	shadowMap->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	// get the world, view, and projection matrices from the camera and d3d objects.

	light->generateViewMatrix();
	XMMATRIX lightViewMatrix = light->getViewMatrix();
	XMMATRIX lightProjectionMatrix = light->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	// Render Terrain
	mesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, TerrainHeight);
	depthShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	// Set back buffer as render target and reset view port.
    renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::SeconddepthPass()
{
	// Set the render target to be the render to texture.
	SecondshadowMap->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());
////////////////////////////////////////////////////////////////////////////////////////////////
	light5->generateViewMatrix();
	light5->generateProjectionMatrix(1.0f, 100);

	XMMATRIX lightViewMatrix = light5->getViewMatrix();
	XMMATRIX lightProjectionMatrix = light5->getProjectionMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	XMMATRIX  CubetranslationMatrix = XMMatrixTranslation(CubeLocation.x, CubeLocation.y, CubeLocation.z);
	XMMATRIX  CubescaleMatrix = XMMatrixScaling(CubeScale.x, CubeScale.y, CubeScale.z);

	worldMatrix = CubetranslationMatrix * CubescaleMatrix;
	Cube->sendData(renderer->getDeviceContext());
	SecondDepthShader->SecondShadowShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	SecondDepthShader->render(renderer->getDeviceContext(), Cube->getIndexCount());

	///////////////////////////////////////////////////////////////////////////////////////
	XMMATRIX FloorworldMatrix = light5->getViewMatrix();
	XMMATRIX FloorviewMatrix = light5->getProjectionMatrix();
	XMMATRIX FloorprojectionMatrix = renderer->getProjectionMatrix();

	XMMATRIX  FloortranslationMatrix = XMMatrixTranslation(FloorLocation.x, FloorLocation.y, FloorLocation.z);
	XMMATRIX  FloorscaleMatrix = XMMatrixScaling(FloorScale.x, FloorScale.y, FloorScale.z);

	FloorworldMatrix = FloortranslationMatrix * FloorscaleMatrix;
	Floor->sendData(renderer->getDeviceContext());
	SecondDepthShader->SecondShadowShaderParameters(renderer->getDeviceContext(), FloorworldMatrix, FloorviewMatrix, FloorprojectionMatrix);
	SecondDepthShader->render(renderer->getDeviceContext(), Floor->getIndexCount());

	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}


void App1::LightPass() {

	if (wireframeToggle)
	{
		renderer->beginScene(0.0f, 0.0f, 0.0f, 1.0f);

	}
	else {
		LightTexture->setRenderTarget(renderer->getDeviceContext());
	    LightTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);	// Generate the view matrix based on the camera's position.
	}
	
	camera->update();
	////GUI FOR LIGHTS 
	ImGui::Begin("Light Controls");
	light->LightUI(1);
	light2->LightUI(2);
	light3->LightUI(3);
	light4->LightUI(4);
	light5->LightUI(5);

	ImGui::End();
	///////////////////
	
	// TRASFORMATION MATRIXES
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();
	XMFLOAT3 Cameraposition = camera->getPosition();
	
	mesh->sendData(renderer->getDeviceContext());
	terrainshader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, Cameraposition,  textureMgr, shadowMap->getDepthMapSRV(), LightsVector, TerrainHeight,camera->getPosition());
	terrainshader->render(renderer->getDeviceContext(), mesh->getIndexCount());
	
	//////basic physics for snow
	velocity += gravity; 
	velocity *= drag;    

	location -= velocity; // Update location based on the velocity

	// TRASFORMATION MATRIXES
	XMMATRIX worldMatrix1 = renderer->getWorldMatrix();
	XMMATRIX viewMatrix1 = camera->getViewMatrix();
	XMMATRIX projectionMatrix1 = renderer->getProjectionMatrix();

	XMMATRIX scaleMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f); 

	// Define the translation matrix
	XMMATRIX translationMatrix = XMMatrixTranslation(randomHorizontalMovement, location, 0.0f);
	worldMatrix1 = translationMatrix * scaleMatrix;

	SnowPoints->sendData(renderer->getDeviceContext());
	Snowshader->setShaderParameters(renderer->getDeviceContext(), worldMatrix1, viewMatrix1, projectionMatrix1);
	Snowshader->render(renderer->getDeviceContext(), SnowPoints->getIndexCount());


	// TRASFORMATION MATRIXES

	XMMATRIX worldMatrix2 = renderer->getWorldMatrix();
	XMMATRIX viewMatrix2 = camera->getViewMatrix();
	XMMATRIX projectionMatrix2 = renderer->getProjectionMatrix();


	XMMATRIX translationMatrix1 = XMMatrixTranslation(ModelLocation.x, ModelLocation.y, ModelLocation.z);
	XMMATRIX scaleMatrix1 = XMMatrixScaling(ModelScale.x, ModelScale.y, ModelScale.z);

	worldMatrix2 = translationMatrix1 * scaleMatrix1;
	BrickSphere->sendData(renderer->getDeviceContext());
	modelShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix2, viewMatrix2, projectionMatrix2, camera->getPosition(), textureMgr, ModelLightsVector, SecondshadowMap->getDepthMapSRV(), L"Brick_Base", L"Brick_Height", L"Brick_Metal", L"Brick_Rough");
	modelShader->render(renderer->getDeviceContext(), BrickSphere->getIndexCount());

	///////////////////////////////////////////////////////////////////////////////////////
	XMMATRIX FloorworldMatrix = renderer->getWorldMatrix();
	XMMATRIX FloorviewMatrix = camera->getViewMatrix();
	XMMATRIX FloorprojectionMatrix = renderer->getProjectionMatrix();

	XMMATRIX  FloortranslationMatrix = XMMatrixTranslation(FloorLocation.x, FloorLocation.y, FloorLocation.z);
	XMMATRIX  FloorscaleMatrix = XMMatrixScaling(FloorScale.x, FloorScale.y, FloorScale.z);

	FloorworldMatrix = FloortranslationMatrix * FloorscaleMatrix;
	Floor->sendData(renderer->getDeviceContext());
	modelShader->setShaderParameters(renderer->getDeviceContext(), FloorworldMatrix, FloorviewMatrix, FloorprojectionMatrix, camera->getPosition(), textureMgr, ModelLightsVector, SecondshadowMap->getDepthMapSRV(),L"Wood_Base", L"Wood_Height", L"Wood_Metal", L"Wood_Rough");
	modelShader->render(renderer->getDeviceContext(), Floor->getIndexCount());
	///////////////////////////////////////////////////////////////////////////////////////




	///////////////////////////////////////////////////////////////////////////////////////
	XMMATRIX CubeworldMatrix = renderer->getWorldMatrix();
	XMMATRIX CubeviewMatrix = camera->getViewMatrix();
	XMMATRIX CubeprojectionMatrix = renderer->getProjectionMatrix();

	XMMATRIX  CubetranslationMatrix = XMMatrixTranslation(CubeLocation.x, CubeLocation.y, CubeLocation.z);
	XMMATRIX  CubescaleMatrix = XMMatrixScaling(CubeScale.x, CubeScale.y, CubeScale.z);

	CubeworldMatrix = CubetranslationMatrix * CubescaleMatrix;
	Cube->sendData(renderer->getDeviceContext());
	modelShader->setShaderParameters(renderer->getDeviceContext(), CubeworldMatrix, CubeviewMatrix, CubeprojectionMatrix, camera->getPosition(), textureMgr, ModelLightsVector, SecondshadowMap->getDepthMapSRV(),L"Brick_Base", L"Brick_Height", L"Brick_Metal", L"Brick_Rough");
	modelShader->render(renderer->getDeviceContext(), Cube->getIndexCount());
	///////////////////////////////////////////////////////////////////////////////////////



	XMMATRIX worldMatrix3 = renderer->getWorldMatrix();

	XMMATRIX translationMatrix2 = XMMatrixTranslation(light3->getPosition().x, light3->getPosition().y, light3->getPosition().z);
	XMMATRIX scaleMatrix2 = XMMatrixScaling(1, 1, 1);
	worldMatrix3 = translationMatrix2 * scaleMatrix2;
	Sphere->sendData(renderer->getDeviceContext());
	LightShader->BASICShaderParameters(renderer->getDeviceContext(), worldMatrix3, viewMatrix2, projectionMatrix2);
	LightShader->render(renderer->getDeviceContext(), Sphere->getIndexCount());
	///////////////////////////////////////////////////////////////////////////////////////




	XMMATRIX worldMatrix4 = renderer->getWorldMatrix();

	XMMATRIX translationMatrix3 = XMMatrixTranslation(light4->getPosition().x, light4->getPosition().y, light4->getPosition().z);
	XMMATRIX scaleMatrix3 = XMMatrixScaling(1, 1, 1);
	worldMatrix4 = translationMatrix3 * scaleMatrix3;

	Sphere2->sendData(renderer->getDeviceContext());
	LightShader->BASICShaderParameters(renderer->getDeviceContext(), worldMatrix4, viewMatrix2, projectionMatrix2);
	LightShader->render(renderer->getDeviceContext(), Sphere2->getIndexCount());
	///////////////////////////////////////////////////////////////////////////////////////


	XMMATRIX worldMatrix5 = renderer->getWorldMatrix();

	XMMATRIX translationMatrix5 = XMMatrixTranslation(light5->getPosition().x, light5->getPosition().y, light5->getPosition().z);
	XMMATRIX scaleMatrix5 = XMMatrixScaling(1, 1, 1);
	worldMatrix5 = translationMatrix5 * scaleMatrix5;

	Sphere3->sendData(renderer->getDeviceContext());
	LightShader->BASICShaderParameters(renderer->getDeviceContext(), worldMatrix5, viewMatrix2, projectionMatrix2);
	LightShader->render(renderer->getDeviceContext(), Sphere3->getIndexCount());
	///////////////////////////////////////////////////////////////////////////////////////




	renderer->setBackBufferRenderTarget();

}

void App1::PrepforBloom()
{
	
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	float screenSizeX = (float)ExtractLightsforBloomTexture->getTextureWidth();
	float screenSizeY = (float)ExtractLightsforBloomTexture->getTextureWidth();

	ExtractLightsforBloomTexture->setRenderTarget(renderer->getDeviceContext());
	ExtractLightsforBloomTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = ExtractLightsforBloomTexture->getOrthoMatrix();

	// Render extracting lights
	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	extractlightsforBloomShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, LightTexture->getShaderResourceView(), textureMgr, screenSizeX, screenSizeY);
	extractlightsforBloomShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();

}

 void App1::BlurPass(RenderTexture* InputTexture, RenderTexture* FirstTexture, RenderTexture* SecondTexture)
{

	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	float screenSizeX = (float)FirstTexture->getTextureWidth();
	float screenSizeY = (float)FirstTexture->getTextureWidth();

	FirstTexture->setRenderTarget(renderer->getDeviceContext());
	FirstTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = FirstTexture->getOrthoMatrix();

	// Render for Horizontal Blur
	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	HorizontalGaussianblurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, InputTexture->getShaderResourceView(), textureMgr, screenSizeX, screenSizeY);
	HorizontalGaussianblurShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
	///////////////////////////////////////////////////

	SecondTexture->setRenderTarget(renderer->getDeviceContext());
	SecondTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Render for Vertical Blur
	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	VerticalGaussianblurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, FirstTexture->getShaderResourceView(), textureMgr, screenSizeX, screenSizeY);
	VerticalGaussianblurShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();

}


void App1::PostProcesses() {

	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	float screenSizeX = (float)CameraEffectsTexture->getTextureWidth();
	float screenSizeY = (float)CameraEffectsTexture->getTextureWidth();

	CameraEffectsTexture->setRenderTarget(renderer->getDeviceContext());
	CameraEffectsTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = CameraEffectsTexture->getOrthoMatrix();


	// Render for Horizontal Blur
	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	CameraStyleShader1->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, LightTexture->getShaderResourceView(), SixthVerticalGaussianblurTexture->getShaderResourceView(), textureMgr, screenSizeX, screenSizeY);
	CameraStyleShader1->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();


}

void App1::LastPass() {
	renderer->beginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// RENDER THE RENDER TEXTURE SCENE
	// Requires 2D rendering and an ortho mesh.
	renderer->setZBuffer(false);
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering

	orthoMesh->sendData(renderer->getDeviceContext());
	textureshader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, CameraEffectsTexture->getShaderResourceView());
	textureshader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

}

void App1::gui()
{

	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
