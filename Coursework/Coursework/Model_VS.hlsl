cbuffer MatrixBuffer : register(b1)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    float3 CameraLocaiton;
    float Padding;
    matrix lightProjection;
    matrix lightView;
}

struct InputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};


struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float3 ViewVector : TEXCOORD2;
    float4 lightViewVector : TEXCOORD3;

    
};


Texture2D heightmap : register(t11);
SamplerState samplerState : register(s0);

OutputType main(InputType input)
{
    OutputType output;
    
    float4 worldPosition = mul(float4(input.position,1.0f), worldMatrix);
    float4 viewPosition = mul(worldPosition, viewMatrix);
    output.position = mul(viewPosition, projectionMatrix);
    
    output.ViewVector = normalize(CameraLocaiton - worldPosition.xyz);
    
    output.lightViewVector = mul(float4(input.position, 1.0f), worldMatrix);
    output.lightViewVector = mul(output.lightViewVector, lightView);
    output.lightViewVector = mul(output.lightViewVector, lightProjection);
    
    output.tex = input.tex;
    output.normal = input.normal;
    output.worldPosition = worldPosition.xyz;

    return output;
}