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

    float BrightnesThresh;
    float3 Paddding;
    
};

float3 reinhard_extended(float3 v, float max_white)
{
    float max_white_sq = max_white * max_white;
    float3 numerator = v * (1.0f + v / max_white_sq);
    return numerator / (1.0f + v);
}


float4 main(InputType input) : SV_TARGET
{
    float4 textureColor = texture0.Sample(Sampler0, input.tex);
    float4 BrightColor;
    
    float brightness = dot(textureColor.rgb, float3(0.2126, 0.7152, 0.0722));
    
    if (brightness > BrightnesThresh)
    {
        textureColor.rgb = pow(textureColor.rgb, (1.0 / 2.2));
        
        BrightColor = float4(textureColor.rgb, 1.0);

    }
    else
    {
        BrightColor = float4(0.0, 0.0, 0.0, 0.0);

    }
    
    return BrightColor;
}