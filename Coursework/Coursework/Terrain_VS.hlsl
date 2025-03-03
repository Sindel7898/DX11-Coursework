// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader

cbuffer ManniPulationBuffer : register(b1)
{
    float Height;
    float3 ManniPaddng;
};


Texture2D heightmap : register(t0);
SamplerState samplerState : register(s0);

struct InputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

OutputType main(InputType input)
{
    OutputType output;

    float heightCenter = heightmap.SampleLevel(samplerState, input.tex, 0).r;
    
    // Displace the Y position using the heightmap data
    input.position.y += heightCenter * Height;
   
    output.normal = input.normal;
    
	// Store the texture coordinates for the pixel shader.
    output.position = input.position;
    output.tex = input.tex;


    return output;
}