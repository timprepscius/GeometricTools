uniform float2 SpaceParam;    // (dx, dy)
uniform float3 EpsilonParam;  // (epsilonX, epsilonY, epsilon0)

sampler2D DivergenceSampler = sampler_state
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

void p_PoissonSolver
(
    in float2 vertexTCoord : TEXCOORD0,
    out float4 pixelColor : COLOR
)
{
    float divergence = tex2D(DivergenceSampler, vertexTCoord).x;

    float poissonPZ = tex2D(PoissonSampler,
        float2(vertexTCoord.x + SpaceParam.x, vertexTCoord.y)).x;

    float poissonMZ = tex2D(PoissonSampler,
        float2(vertexTCoord.x - SpaceParam.x, vertexTCoord.y)).x;

    float poissonZP = tex2D(PoissonSampler,
        float2(vertexTCoord.x, vertexTCoord.y + SpaceParam.y)).x;

    float poissonZM = tex2D(PoissonSampler,
        float2(vertexTCoord.x, vertexTCoord.y - SpaceParam.y)).x;

    pixelColor = EpsilonParam.z*divergence +
        EpsilonParam.x*(poissonPZ + poissonMZ) +
        EpsilonParam.y*(poissonZP + poissonZM);
}
