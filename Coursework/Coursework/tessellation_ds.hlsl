// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix LightviewMatrix;
    matrix LightprojectionMatrix;
    float3 CameraPosition;
    float Padding;
}

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

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
    float3 viewVector : TEXCOORD2;
    float4 lightViewVector: TEXCOORD3;
};


[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
    float3 vertexPosition;
    OutputType output;
 
    // Determine the position of the new vertex.
	// Invert the y and Z components of uvwCoord as these coords are generated in UV space and therefore y is positive downward.
	// Alternatively you can set the output topology of the hull shader to cw instead of ccw (or vice versa).

    float3 V1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
    float3 V2 = lerp(patch[3].position, patch[2].position, uvwCoord.y);
    
    vertexPosition = lerp(V1, V2, uvwCoord.x);
    
    // Calculate the position of the new vertex against the world, view, and projection matrices.
    float4 worldPosition = mul(float4(vertexPosition, 1.0f), worldMatrix);
    float4 viewPosition = mul(worldPosition, viewMatrix);
    output.position = mul(viewPosition, projectionMatrix);
    
    output.viewVector = normalize(CameraPosition - worldPosition.xyz);
    
    output.lightViewVector = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.lightViewVector = mul(output.lightViewVector, LightviewMatrix);
    output.lightViewVector = mul(output.lightViewVector, LightprojectionMatrix);
    
    
    // Send the input color into the pixel shader.
    output.normal = patch[0].normal;
    output.worldPosition = worldPosition;
    output.tex = lerp(lerp(patch[0].tex, patch[1].tex, uvwCoord.y), lerp(patch[3].tex, patch[2].tex, uvwCoord.y), uvwCoord.x);

    return output;
}

