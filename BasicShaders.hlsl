cbuffer transforms : register(b0)
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
    float3 diffuseLightPosition;
    float4 diffuseLightColor;
};

struct VS_INPUT
{
    float3 Pos : POSITION; //position
    float3 Norm : NORMAL; //normal
    float2 Tex : TEXCOORD0; //texture coordinate
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Diffuse : COLOR0;
    float2 Tex : TEXCOORD1;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul( float4(input.Pos,1), World);
    output.Pos = mul( output.Pos, View);
    output.Pos = mul( output.Pos, Projection);

    // Calculate per pixel diffuse light adjustment to color
    // Light position defined in World coordinates
    float3 worldAdjustedNormal = normalize(mul(input.Norm, (float3x3)World));
    float3 normalizedLightPosition = normalize(diffuseLightPosition);
    float diffuseLightMultiplier = saturate(dot(worldAdjustedNormal, normalizedLightPosition));

    output.Diffuse = float4(1.0f, 0.0f, 0.0f, 1.0f)*diffuseLightMultiplier;
    output.Tex = input.Tex;
    
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input) : SV_Target
{
    //calculate lighting assuming light color is <1,1,1,1>
    float4 outputColor = input.Diffuse;
    outputColor.a = 1;
    return outputColor;
}