Texture2D texture0 : register(t5);
SamplerState Sampler0 : register(s5);

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

cbuffer LightBuffer : register(b0)
{
    float screenWidth;
    float screenHeight;
    float2 Padding;

};


float4 main(InputType input) : SV_TARGET
{
    float4 textureColor = texture0.Sample(Sampler0, input.tex);

    float2 texelSize = (1 / (screenWidth * 0.4), 1 / (screenHeight * 0.4));
    
    float weights[6] = { 0.06136, 0.125, 0.24477, 0.38774, 0.24477, 0.125 };

// Initialize color to zero
    float4 color = float4(0.0, 0.0, 0.0, 1.0);

    
        color += texture0.Sample(Sampler0, input.tex + float2(0.0,  1 * texelSize.y)) * weights[1];
        color += texture0.Sample(Sampler0, input.tex + float2(0.0,  2 * texelSize.y)) * weights[2];
        color += texture0.Sample(Sampler0, input.tex + float2(0.0,  3 * texelSize.y)) * weights[3];
        color += texture0.Sample(Sampler0, input.tex + float2(0.0,  4 * texelSize.y)) * weights[4];
        color += texture0.Sample(Sampler0, input.tex + float2(0.0,  5 * texelSize.y)) * weights[5];

        color += texture0.Sample(Sampler0, input.tex + float2(0.0,  0 * texelSize.y)) * weights[0];

       color += texture0.Sample(Sampler0, input.tex + float2(0.0, -1 * texelSize.y)) * weights[1];
       color += texture0.Sample(Sampler0, input.tex + float2(0.0, -2 * texelSize.y)) * weights[2];
       color += texture0.Sample(Sampler0, input.tex + float2(0.0, -3 * texelSize.y)) * weights[3];
       color += texture0.Sample(Sampler0, input.tex + float2(0.0, -4 * texelSize.y)) * weights[4];
       color += texture0.Sample(Sampler0, input.tex + float2(0.0, -5 * texelSize.y)) * weights[5];
   
    
    return color;
}