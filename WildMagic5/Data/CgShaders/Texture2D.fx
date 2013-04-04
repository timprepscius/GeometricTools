// The application can changes these parameters at run time.
sampler2D BaseSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Nearest;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

void v_Texture2D
(
    in float3 modelPosition : POSITION,
    in float2 modelTCoord : TEXCOORD0,
    out float4 clipPosition : POSITION,
    out float2 vertexTCoord : TEXCOORD0,
    uniform float4x4 PVWMatrix
)
{
    // Transform the position from model space to clip space.
    clipPosition = mul(PVWMatrix, float4(modelPosition,1.0f));

    // Pass through the texture coordinate.
    vertexTCoord = modelTCoord;
}

void p_Texture2D
(
    in float2 vertexTCoord : TEXCOORD0,
    out float4 pixelColor : COLOR
)
{
    // Sample the texture image.
    pixelColor = tex2D(BaseSampler, vertexTCoord);
}
