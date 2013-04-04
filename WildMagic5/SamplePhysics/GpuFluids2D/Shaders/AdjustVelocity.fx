uniform float4 SpaceParam;   // (dx, dy, halfDivDx, halfDivDy)

sampler2D StateSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Nearest;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

sampler2D PoissonSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Nearest;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

void p_AdjustVelocity
(
    in float2 vertexTCoord : TEXCOORD0,
    out float4 pixelColor : COLOR
)
{
    float4 state = tex2D(StateSampler, vertexTCoord);

    float poissonXP = tex2D(PoissonSampler,
        float2(vertexTCoord.x + SpaceParam.x, vertexTCoord.y)).x;

    float poissonXM = tex2D(PoissonSampler,
        float2(vertexTCoord.x - SpaceParam.x, vertexTCoord.y)).x;

    float poissonYP = tex2D(PoissonSampler,
        float2(vertexTCoord.x, vertexTCoord.y + SpaceParam.y)).y;

    float poissonYM = tex2D(PoissonSampler,
        float2(vertexTCoord.x, vertexTCoord.y - SpaceParam.y)).y;

    float diffX = poissonXP - poissonXM;
    float diffY = poissonYP - poissonYM;

    pixelColor.x = state.x + SpaceParam.z*diffX;
    pixelColor.y = state.y + SpaceParam.w*diffY;
    pixelColor.z = 0.0f;
    pixelColor.w = state.w;
}
