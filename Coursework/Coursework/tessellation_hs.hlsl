// Tessellation Hull Shader
// Prepares control points for tessellation
struct InputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct OutputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

cbuffer CameraBuffer : register(b0)
{
    float3 CameraPosition;
    float Padding;
};


ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{    
    ConstantOutputType output;

    //calculate distance from the center of the patch to the camera
    float3 patchCenter = (inputPatch[0].position + inputPatch[1].position + inputPatch[2].position + inputPatch[3].position) / 4.0;
    float d = distance(patchCenter, CameraPosition);
    
    const float d0 = 20.0f;
    const float d1 = 70.0f;
    float tess = 15.0f  * saturate((d1 - d) / (d1 - d0));
    // Clamp the tessellation value between 1.0 and 10.0 to prevent excessive tessellation
    tess = clamp(tess, 1.0f, 10.0f); 
    
   // Set tessellation constants for the edges of the patch
    output.edges[0] = max(tess, 1);;
    output.edges[1] = max(tess, 1);;
    output.edges[2] = max(tess, 1);;
    output.edges[3] = max(tess, 1);;
    
  // Set tessellation constants for the inside of the patch (triangles)
    output.inside[0] = max(tess * 0.5, 1);;
    output.inside[1] = max(tess * 0.5, 1);;
    
    
 
    return output;
}


// Main hull shader function
[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 4> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    OutputType output;

    // Pass through the control point data
    output.position = patch[pointId].position;
    output.normal = patch[pointId].normal;
    output.tex = patch[pointId].tex;

    return output;
}