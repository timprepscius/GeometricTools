uniform float4 SpaceParam;  // (dx, dy, 1/width, 1/height)
uniform float3 TimeParam;   // (dtDivDx, dtDivDy, dt)
uniform float4 ViscParam;   // (denLX, denLY, velLX, velLY)

sampler2D StateSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Nearest;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

sampler2D AdvectionSampler = sampler_state
{
   MinFilter = Linear;
   MagFilter = Nearest;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

sampler2D SourceSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Nearest;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

void p_FluidUpdate
(
    in float2 vertexTCoord : TEXCOORD0,
    out float4 pixelColor : COLOR
)
{
    // Sample states at (x,y), (x+dx,y), (x-dx,y), (x,y+dy), (x,y-dy).
    float4 stateZZ = tex2D(StateSampler, vertexTCoord);

    float4 statePZ = tex2D(StateSampler,
        float2(vertexTCoord.x + SpaceParam.x, vertexTCoord.y));

    float4 stateMZ = tex2D(StateSampler,
        float2(vertexTCoord.x - SpaceParam.x, vertexTCoord.y));

    float4 stateZP = tex2D(StateSampler,
        float2(vertexTCoord.x, vertexTCoord.y + SpaceParam.y));

    float4 stateZM = tex2D(StateSampler,
        float2(vertexTCoord.x, vertexTCoord.y - SpaceParam.y));

    // Compute texture coordinate for advection.
    float xAdv = vertexTCoord.x - TimeParam.x*SpaceParam.z*stateZZ.x;
    float yAdv = vertexTCoord.y - TimeParam.y*SpaceParam.w*stateZZ.y;
    float4 stateAdv = tex2D(AdvectionSampler, float2(xAdv, yAdv));

    // Sample the source state.
    float4 source = tex2D(SourceSampler, vertexTCoord);

    // Update density.
    float denSDx = statePZ.w - 2.0f*stateZZ.w + stateMZ.w;
    float denSDy = stateZP.w - 2.0f*stateZZ.w + stateZM.w;
    pixelColor.w = stateAdv.w + 2.0f*(ViscParam.x*denSDx +
        ViscParam.y*denSDy + TimeParam.z*source.w);

    // Update velocity.
    float2 velSDx = statePZ.xy - 2.0f*stateZZ.xy + stateMZ.xy;
    float2 velSDy = stateZP.xy - 2.0f*stateZZ.xy + stateZM.xy;
    pixelColor.xy = stateAdv.xy + 2.0f*(ViscParam.z*velSDx +
        ViscParam.w*velSDy + TimeParam.z*source.xy);

    pixelColor.z = 0.0f;
}
