uniform float4 SpaceParam;  // (dx, dy, halfDivDx, halfDivDy)

sampler2D StateSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Nearest;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

void p_Divergence
(
    in float2 vertexTCoord : TEXCOORD0,
    out float4 pixelColor : COLOR
)
{
    float velocityXP = tex2D(StateSampler,
        float2(vertexTCoord.x + SpaceParam.x, vertexTCoord.y)).x;

    float velocityXM = tex2D(StateSampler,
        float2(vertexTCoord.x - SpaceParam.x, vertexTCoord.y)).x;

    float velocityYP = tex2D(StateSampler,
        float2(vertexTCoord.x, vertexTCoord.y + SpaceParam.y)).y;

    float velocityYM = tex2D(StateSampler,
        float2(vertexTCoord.x, vertexTCoord.y - SpaceParam.y)).y;

    pixelColor =
        SpaceParam.z*(velocityXP - velocityXM) +
        SpaceParam.w*(velocityYP - velocityYM);
}
