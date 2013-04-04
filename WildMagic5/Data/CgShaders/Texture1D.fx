// The application can changes these parameters at run time.
sampler1D BaseSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Nearest;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

void v_Texture1D
(
    in float3 modelPosition : POSITION,
    in float modelTCoord : TEXCOORD0,
    out float4 clipPosition : POSITION,
    out float vertexTCoord : TEXCOORD0,
    uniform float4x4 PVWMatrix
)
{
    // Transform the position from model space to clip space.
    clipPosition = mul(PVWMatrix, float4(modelPosition,1.0f));

    // Pass through the texture coordinate.
    vertexTCoord = modelTCoord;
}

void p_Texture1D
(
    in float vertexTCoord : TEXCOORD0,
    out float4 pixelColor : COLOR
)
{
    // Sample the texture image.
    pixelColor = tex1D(BaseSampler, vertexTCoord);
}
