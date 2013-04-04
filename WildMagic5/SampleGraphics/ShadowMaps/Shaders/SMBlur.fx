sampler2D BaseSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Linear;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

void v_SMBlur
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
    
    // Pass through the texture coordinates.
    vertexTCoord = modelTCoord;
}

void p_SMBlur
(
    in float2 vertexTCoord : TEXCOORD0,
    out float4 pixelColor : COLOR,
    uniform float3 Weights[11],
    uniform float2 Offsets[11]
)
{
    pixelColor = 0.0f;
    for (int i = 0; i < 11; ++i)
    {
        float2 tcoord = vertexTCoord + Offsets[i];
        pixelColor.rgb += Weights[i]*tex2D(BaseSampler, tcoord).rgb;
    }
    pixelColor.a = 1.0f;
}
