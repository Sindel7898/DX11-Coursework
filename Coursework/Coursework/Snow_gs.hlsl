cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
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
};

[maxvertexcount(4)]
void main(point InputType input[1], inout TriangleStream<OutputType> outstream)
{
    OutputType output;

    static float3 Generatedpositions[4] =
    {
    float3(-0.2, 0.2, -0.2),
	float3(-0.2, -0.2, -0.2),
	float3(0.2, 0.2, -0.2),
	float3(0.2, -0.2, -0.2),
    };
    
    
    for (int i = 0; i < 4; i++)
    {
        float4 Position = float4(Generatedpositions[i], 1.0f) + float4(input[0].position, 1.0f);
        output.position = mul(Position, worldMatrix);
        output.position = mul(output.position, viewMatrix);
        output.position = mul(output.position, projectionMatrix);
        output.tex = input[0].tex;
        output.normal = input[0].normal;
        outstream.Append(output);
        
    }
    outstream.RestartStrip();
}