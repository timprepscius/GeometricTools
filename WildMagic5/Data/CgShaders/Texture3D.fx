// The application can changes these parameters at run time.
sampler3D BaseSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Nearest;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

void v_Texture3D
(
    in float3 modelPosition : POSITION,
    in float3 modelTCoord : TEXCOORD0,
    out float4 clipPosition : POSITION,
    out float3 vertexTCoord : TEXCOORD0,
    uniform float4x4 PVWMatrix
)
{
    // Transform the position from model space to clip space.
    clipPosition = mul(PVWMatrix, float4(modelPosition,1.0f));

    // Pass through the texture coordinate.
    vertexTCoord = modelTCoord;
}

void p_Texture3D
(
    in float3 vertexTCoord : TEXCOORD0,
    out float4 pixelColor : COLOR
)
{
    // Sample the texture image.
    pixelColor = tex3D(BaseSampler, vertexTCoord);
}
