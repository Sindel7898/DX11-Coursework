// Light class
// Holds data that represents a single light source
#include "light.h"
#include <imGUI\imgui.h>
#include <string>

// create view matrix, based on light position and lookat. Used for shadow mapping.
void Light::generateViewMatrix()
{
	// default up vector
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	if (direction.y == 1 || (direction.x == 0 && direction.z == 0))
	{
		up = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0);
	}
	else if (direction.y == -1 || (direction.x == 0 && direction.z == 0))
	{
		up = XMVectorSet(0.0f, 0.0f, -1.0f, 1.0);
	}
	//XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	XMVECTOR dir = XMVectorSet(direction.x, direction.y, direction.z, 1.0f);
	XMVECTOR right = XMVector3Cross(dir, up);
	up = XMVector3Cross(right, dir);
	// Create the view matrix from the three vectors.
	viewMatrix = XMMatrixLookAtLH(position, position + dir, up);
}

// Create a projection matrix for the (point) light source. Used in shadow mapping.
void Light::generateProjectionMatrix(float screenNear, float screenFar)
{
	float fieldOfView, screenAspect;

	// Setup field of view and screen aspect for a square light source.
	fieldOfView = (float)XM_PI / 2.0f;
	screenAspect = 1.0f;

	// Create the projection matrix for the light.
	projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenFar);
}

// Create orthomatrix for (directional) light source. Used in shadow mapping.
void Light::generateOrthoMatrix(float screenWidth, float screenHeight, float near, float far)
{
	orthoMatrix = XMMatrixOrthographicLH(screenWidth, screenHeight, near, far);
}

void Light::setAmbientColour(float red, float green, float blue, float alpha)
{
	ambientColour = XMFLOAT4(red, green, blue, alpha);
}

void Light::setDiffuseColour(float red, float green, float blue, float alpha)
{
	diffuseColour = XMFLOAT4(red, green, blue, alpha);
}

void Light::setDirection(float x, float y, float z)
{
	direction = XMFLOAT3(x, y, z);
}

void Light::setSpecularColour(float red, float green, float blue, float alpha)
{
	specularColour = XMFLOAT4(red, green, blue, alpha);
}

void Light::setSpecularPower(float power)
{
	specularPower = power;
}

void Light::setPosition(float x, float y, float z)
{
	position = XMVectorSet(x, y, z, 1.0f);
}

XMFLOAT4 Light::getAmbientColour()
{
	return ambientColour;
}

XMFLOAT4 Light::getDiffuseColour()
{
	return diffuseColour;
}


XMFLOAT3 Light::getDirection()
{
	return direction;
}

XMFLOAT4 Light::getSpecularColour()
{
	return specularColour;
}


float Light::getSpecularPower()
{
	return specularPower;
}

XMFLOAT3 Light::getPosition()
{
	XMFLOAT3 temp(XMVectorGetX(position), XMVectorGetY(position), XMVectorGetZ(position));
	return temp;
}

void Light::setLookAt(float x, float y, float z)
{
	lookAt = XMVectorSet(x, y, z, 1.0f);
}


XMMATRIX Light::getViewMatrix()
{
	return viewMatrix;
}

XMMATRIX Light::getProjectionMatrix()
{
	return projectionMatrix;
}

XMMATRIX Light::getOrthoMatrix()
{
	return orthoMatrix;
}

float Light::getLightIntensity()
{
	return LightIntensity;
}

int Light::GetLightType()
{
	return LightTypePick;
}

XMFLOAT2 Light::GetConeParams()
{
	return ConeParam;
}


void Light::LightUI(int LightNumber)
{


	// Array of light types for selection
	const char* items[] = { "Point Light", "Spot Light" ,  "Directional Light"   };

	// Combo box for selecting light type
	if (ImGui::Combo(("Select Option" + std::to_string(LightNumber)).c_str(), &LightTypePick, items, IM_ARRAYSIZE(items))) {
		ImGui::Text("Selected option: %s", items[LightTypePick]);
	}

	// Slider for setting the light's position
	ImGui::SliderFloat3(("Light " + std::to_string(LightNumber) + " Position").c_str(), &Newposition.x, -200.0f, 200.0f);

	// Display cone parameters if light type is Spot Light
	if (LightTypePick == 1)
	{
		ImGui::SliderFloat2(("Light " + std::to_string(LightNumber) + " Cone Params").c_str(), &ConeParam.x, -150.0f, 150.0f);

	}

	// Display direction parameters for Spot and Directional Lights
	if (LightTypePick == 1 || LightTypePick == 2)
	{
		ImGui::SliderFloat3(("Light " + std::to_string(LightNumber) + " Direction").c_str(), &direction.x, -1.0f, 1.0f);

	}

	// Sliders for setting the light's color
	ImGui::ColorEdit3(("Light " + std::to_string(LightNumber) + " Color").c_str(), &diffuseColour.x);
	ImGui::ColorEdit3(("Light" + std::to_string(LightNumber) + " Specular Color").c_str(), &specularColour.x);

	// Sliders for setting the light's intensity and specular power
	ImGui::SliderFloat(("Light " + std::to_string(LightNumber) + " Intensity").c_str(), &LightIntensity, 1, 90.0f);
	ImGui::SliderFloat(("Light " + std::to_string(LightNumber) + " Specular Intensity").c_str(), &specularPower, 1, 40.0f);
	// Spacing between UI elements
	ImGui::Spacing();
	ImGui::Spacing();

	// Update light properties
	setPosition(Newposition.x, Newposition.y, Newposition.z);
	setDirection(direction.x, direction.y, direction.z);
	setDiffuseColour(diffuseColour.x, diffuseColour.y, diffuseColour.z, 1.0f);
}

void Light::SetLightIntensity(int LightNumber)
{
	LightIntensity = LightNumber;
}

void Light::SetLightType(int LightType)
{

	LightTypePick = LightType;
}

void Light::SetConeParams(float innerCone, float outtercone)
{
	ConeParam = { innerCone ,outtercone };
}

void Light::SetLghtIntensity(int lightIntensity)
{
	LightIntensity = lightIntensity;
}

void Light::SetSpecularPower(int SpecularIntensity)
{
	specularPower = SpecularIntensity;
}
