// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"
#include "TerainShader.h"
#include "CameraStyleShader.h"
#include "TextureShader.h"
#include "ExtratLightForBlom.h"
#include "HorizontalGaussianblur.h"
#include "DepthShader.h"
#include "TesselationPlane.h"
#include "SnowShader.h"
#include "PointList.h"
#include "ModelShader.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void depthPass();
	void SeconddepthPass();
	void LightPass();
	void PrepforBloom();
	void BlurPass(RenderTexture* InputTexture, RenderTexture* TEXTURE, RenderTexture* SecondTexture);
	//void SecondBlurPass();

	//void ThirdBlurPass();
	//void ForthBlurPass();
	//void FifthBlurPass();


	float angle = 1.0f;
	float rotationSpeed = 0.1;
	void PostProcesses();
	void LastPass();
	void gui();


private:

	TesselationPlane* mesh;
	SphereMesh* Sphere;
	SphereMesh* Sphere2;
	SphereMesh* Sphere3;

	SphereMesh* BrickSphere;
	PlaneMesh* Floor;
	CubeMesh* Cube;
	float location = 10.0f; // Start from above the ground level
	 const float gravity = 0.0005f;
	  float drag = 0.98f; // Slight air resistance
	  float velocity = 0.0f;



	float randomHorizontalMovement = (rand() % 100 - 50) * 0.01f; // Random horizontal shift
	OrthoMesh* orthoMesh;

	TerainShader* terrainshader;
	CameraStyleShader* CameraStyleShader1;
	ExtratLightForBlom* extractlightsforBloomShader;
	HorizontalGaussianblur* HorizontalGaussianblurShader;
	HorizontalGaussianblur* VerticalGaussianblurShader;
	SnowShader* Snowshader;
	TextureShader* textureshader;

	std::unique_ptr<Light> light;
	std::unique_ptr<Light> light2;
	std::unique_ptr<Light> light3;
	std::unique_ptr<Light> light4;
	std::unique_ptr<Light> light5;

	std::vector<Light*> LightsVector;
	std::vector<Light*> ModelLightsVector;

	DepthShader* depthShader;
	DepthShader* SecondDepthShader;

	RenderTexture* LightTexture;
	RenderTexture* ExtractLightsforBloomTexture;
	RenderTexture* HorizontalGaussianblurTexture;
	RenderTexture* VerticalGaussianblurTexture;
	RenderTexture* SecondHorizontalGaussianblurTexture;
	RenderTexture* SecondVerticalGaussianblurTexture;

	RenderTexture* ThirdHorizontalGaussianblurTexture;
	RenderTexture* ThirdVerticalGaussianblurTexture;

	RenderTexture* ForthHorizontalGaussianblurTexture;
	RenderTexture* ForthVerticalGaussianblurTexture;

	RenderTexture* fifthHorizontalGaussianblurTexture;
	RenderTexture* fifthVerticalGaussianblurTexture;

	RenderTexture* SixthHorizontalGaussianblurTexture;
	RenderTexture* SixthVerticalGaussianblurTexture;


	RenderTexture* CameraEffectsTexture;
	float initParticlePosX = 0.0f;
	float initParticlePosY = 0.0f;

	float particleVelocityY = 3.0f;
	float particleVelocityX = 3.0f;

	std::unique_ptr<ShadowMap> shadowMap;
	std::unique_ptr<ShadowMap> SecondshadowMap;


	std::vector<ShadowMap*> ShadowMapVectors;
	XMFLOAT3 ModelLocation{ -0.2f,12.1f,0.0f };
	XMFLOAT3 ModelScale{ 6.0f,6.0f,6.0f };

	XMFLOAT3 FloorLocation{ -16.2f,42.1f,-143.0f };
	XMFLOAT3 FloorScale{ 0.5f,0.5f,0.5f };

	XMFLOAT3 CubeLocation{ 2.532f,8.8f,-8.6f };
	XMFLOAT3 CubeScale{ 4.9f,3.3f,5.0f };

	PointList* SnowPoints;
	ModelShader* modelShader;
	ModelShader* LightShader;

	float TerrainHeight = 123;
};

#endif