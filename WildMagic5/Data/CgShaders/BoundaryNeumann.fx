sampler2D OffsetSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Nearest;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

sampler2D StateSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Nearest;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

void p_BoundaryNeumann
(
    in float2 vertexTCoord : TEXCOORD0,
    out float4 pixelColor : COLOR
)
{
    float2 offset = tex2D(OffsetSampler, vertexTCoord).xy;
    float2 tc = float2(vertexTCoord.x + offset.x, vertexTCoord.y + offset.y);
    float4 state = tex2D(StateSampler, tc);
    pixelColor = state;
}
