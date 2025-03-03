// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry

// Texture and sampler registers
Texture2D texture0 : register(t5);
SamplerState Sampler0 : register(s5);

Texture2D BloomTexture : register(t6);
SamplerState BloomSampler : register(s6);


struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

cbuffer PostPBuffer : register(b0)
{

    float screenWidth;
    float screenHeight;
    float R_offset;
    float G_offset;

    float B_Offset;
    float vignetteStrength;
    float vignetteRadius;
    float FilmGrainStreng;
    float BloomStrengh;
    float3 Padding;

    
};


float4 reinhard(float4 v)
{
    return v / (1.0f + v);
}

float4 main(InputType input) : SV_TARGET
{
      float4 textureColor = texture0.Sample(Sampler0, input.tex);
      float4 Bloom = BloomTexture.Sample(BloomSampler, input.tex);
    
      //// Offset values for RGB channels
      float2 redOffset = float2(R_offset, 0); // Offset for the red channel
      float2 greenOffset = float2(0, G_offset); // No offset for the green channel
      float2 blueOffset = float2(-B_Offset, 0); // Offset for the blue channel

      float4 color = float4(0, 0, 0, 1); // Initialize output color

      color.r = texture0.Sample(Sampler0, input.tex + redOffset).r; // Sample red
      color.g = texture0.Sample(Sampler0, input.tex + greenOffset).g; // Sample green
      color.b = texture0.Sample(Sampler0, input.tex + blueOffset).b; // Sample blu    

      textureColor = lerp(textureColor, color, 1);

      textureColor = lerp(textureColor, Bloom * (BloomStrengh * 0.2), 0.3);

      float2 CenterOfScreen = float2(0.5f, 0.5f);
      float2 offset = input.tex - CenterOfScreen;
      float distance = length(offset);
 

      float vignetteFactor = smoothstep(vignetteRadius, 1.0, distance);
      textureColor.rgb *= (1.0 - vignetteFactor * vignetteStrength);
 

    return textureColor;
}