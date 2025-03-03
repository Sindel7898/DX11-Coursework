// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)
#define PI 3.14159265359

Texture2D HeightMap : register(t1);
Texture2D TerrainTexture : register(t2);
Texture2D ShadowDepthMap1 : register(t3);

SamplerState sampler0 : register(s1);
SamplerState Depthsampler : register(s3);



struct LightData
{

    float3 lightPosition; // Position of the light source

    float padding; // Padding to maintain alignment

    float3 lightDirection; // Direction the light is pointing

    float padding2; // Additional padding

    float4 diffuseColour; // Diffuse color of the light

    float LightIntensity; // Intensity of the light

    float3 padding3; // Further padding

    float LightType; // Type of light (0: point, 1: spot, 2: directional)

    float3 padding4; // Additional padding

    float2 ConeParams; // Spot light cone parameters (inner and outer angle)

    float2 padding5; // Further padding

    float4 SpecularColor; // Specular color of the light

    float specularPower; // Specular power (shininess)

    float3 padding6; // Final padding

};



cbuffer LightBuffer : register(b0)
{
    LightData lights[4]; // Array of light data structures (up to 4 lights)

};

struct InputType
{
    float4 position : SV_POSITION; 
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float3 viewVector : TEXCOORD2;
    float4 lightViewVector : TEXCOORD3;
};
// Schlick's approximation to the Smith-GGX shadowing function
float DistributionGGX(float3 N, float3 H, float a)
{
    float a2 = pow(a, 2);
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = pow(NdotH, 2);

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * pow(denom, 2);

    return nom / denom;
}

// Schlick's approximation to the Smith-GGX shadowing function
float GeometrySchlickGGX(float NdotV, float k)
{
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}

// Geometry Smith function to combine the GGX distribution and Schlick's approximation
float GeometrySmith(float3 N, float3 V, float3 L, float k)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
	
    return ggx1 * ggx2;
}

// Calculate the diffuse lighting based on light direction, normal, diffuse color, and intensity
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse, float lightIntensity)
{
    float NdotL = max(dot(normal, lightDirection), 0.0);
    
    float4 colour = diffuse * NdotL;

   return saturate(colour * lightIntensity);
}

// Fresnel Schlick approximation for reflectance based on the cosine of the angle of incidence
float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);

}

// Retrieve height displacement from the height map
float GetHeightDisplacement(float2 uv)
{
    float offset = HeightMap.SampleLevel(sampler0, uv, 0).r;
    return offset * 132.0f;
}


float3 CalcNormal(float2 uv)
{
    float2 dimensions;
    
    HeightMap.GetDimensions(dimensions.x, dimensions.y);

    float uvoff = 1.0f / min(dimensions.x, dimensions.y);
    
    float HeightN = GetHeightDisplacement(float2(uv.x, uv.y + uvoff)); // Sample height to the north
    float HeightS = GetHeightDisplacement(float2(uv.x, uv.y - uvoff)); // Sample height to the south
    
    float HeightE = GetHeightDisplacement(float2(uv.x + uvoff, uv.y)); // Sample height to the east
    float HeightW = GetHeightDisplacement(float2(uv.x - uvoff, uv.y)); // Sample height to the west

    float worldStep = 100.0f * uvoff;
    
    float3 tan = normalize(float3(2.0f * worldStep, HeightE - HeightW, 0)); // Compute tangent
    
    float3 bitan = normalize(float3(0, HeightN - HeightS, 2.0f * worldStep)); // Compute bitangent
    
    return cross(bitan, tan); // Compute normal as the cross product of the bitangent and tangent
    

}

float3 DoSpotCone(LightData light, float3 L)
{
    float minCos = cos(radians(light.ConeParams.x));//inner angle
    float maxCos = cos(radians(light.ConeParams.y));; //outter angle
    float cosAngle = dot(-normalize(light.lightDirection), L);
    return smoothstep(maxCos, minCos, cosAngle);
}
// Check if UV coordinates are within the valid range (0 to 1)
bool hasDepthData(float2 uv)
{
    if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f)
    {
        return false;
    }
    return true;
}

bool isInShadow(Texture2D sMap, float2 uv, float4 lightViewPosition, float bias)
{
    // Sample the shadow map (get depth of geometry)
    float depthValue = sMap.Sample(Depthsampler, uv).r;
	// Calculate the depth from the light.
    float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
    lightDepthValue -= bias;

	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
    if (lightDepthValue < depthValue)
    {
        return false;
    }
    return true;
}

float2 getProjectiveCoords(float4 lightViewPosition)
{
    // Calculate the projected texture coordinates.
    float2 projTex = lightViewPosition.xy / lightViewPosition.w;
    projTex *= float2(0.5, -0.5);
    projTex += float2(0.5f, 0.5f);
    return projTex;
}



float4 main(InputType input) : SV_TARGET
{
 
    input.normal = CalcNormal(input.tex);
    input.normal = normalize(input.normal);
    

    float4 Ambient = float4(0.1, 0.1, 0.1, 1.0f);
    
    float4 textureColour = TerrainTexture.Sample(sampler0, input.tex);
    float4 heightmaptexturwe = HeightMap.Sample(sampler0, input.tex);

    //metalic value
    float Metallic = 0.2f;
    //Roughness value
    float roughness = 0.6f;
    
    float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

    
    for (int i = 0; i < 2; ++i)
    {
        LightData light = lights[i];
        float3 lightVector;
 
        float2 pTexCoord1 = getProjectiveCoords(input.lightViewVector); // First shadow map

        // Calculate light vector Depending on light type
        if (light.LightType == 2)
        {
            lightVector = -normalize(light.lightDirection);
        }
        else
        {
            lightVector = normalize(light.lightPosition - input.worldPosition);
        }
                 
       
        // Halfway vector between light and view direction
        float3 HAlfWayVector = normalize(normalize(lightVector) + normalize(input.viewVector));
                
        float distance = length(light.lightPosition - input.worldPosition);
         
        // Attenuation factors
        float constantatten = 1.0;
        float linearatten = 0.02; 
        float quadraticatten = 0.005; 
                
        float attenuation = 1.0 / (constantatten + linearatten * distance + quadraticatten * (distance * distance));
      
        float4 lightContribution = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 Diffuse = (0.0f, 0.0f, 0.0f, 0.0f);
               
        // Calculate bias for shadow mapping
        float Bias = max(0.05 * (1.0 - dot(input.normal, light.lightDirection)), 0.005);
        
        // Check if fragment is in shadow
        bool inShadow1 = hasDepthData(pTexCoord1) && isInShadow(ShadowDepthMap1, pTexCoord1, input.lightViewVector, Bias);

        
        if (!(inShadow1)) 
            {
   
                // Fresnel term for reflectance
                float3 F0 = float3(0.04, 0.04, 0.04);
                F0 = lerp(F0, textureColour.rgb, Metallic);
                float3 F = fresnelSchlick(max(dot(lightVector, HAlfWayVector), 0.0), F0);

                // Diffuse lighting calculation
                Diffuse = calculateLighting(lightVector, input.normal, light.diffuseColour, light.LightIntensity);
                
                // Distribution and Geometry terms for specular lighting
                float NDF = DistributionGGX(input.normal, HAlfWayVector, roughness);
                float G = GeometrySmith(input.normal, input.viewVector, lightVector, roughness);
                
                
                // Calculate specular term
                float3 numerator = NDF * G * F;
                float denominator = 4.0 * max(dot(input.normal, input.viewVector), 0.0) * max(dot(input.normal, lightVector), 0.0) + 0.0001;
                float4 specular = float4(numerator / denominator, 1.0f) * light.SpecularColor * light.specularPower;

            
            
                if (light.LightType == 0)
                {
                lightContribution = (Ambient + Diffuse) * attenuation + specular * attenuation;
            }
                else if (light.LightType == 1)
                {
                    float3 SpotEffect = DoSpotCone(light, lightVector);
                    
                lightContribution = (Ambient + Diffuse * float4(SpotEffect, 1.0f)) * attenuation + specular * float4(SpotEffect, 1.0f) * attenuation;
                
                }
                else if (light.LightType == 2)
                {
                  lightContribution = Ambient + Diffuse + specular;
                  
                
                }
                         
        }
         
        
        else
        {
            finalColor += (Ambient * 0.3); // Ambient contribution when in shadow

         }
        
        finalColor += lightContribution; // Accumulate light contributions

    }
    
    return saturate(finalColor) * textureColour; // Apply final color and clamp to [0, 1]


}


