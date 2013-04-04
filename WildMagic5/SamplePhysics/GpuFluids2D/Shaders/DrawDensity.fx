sampler2D StateSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Nearest;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

sampler1D ColorTableSampler = sampler_state
{
   MinFilter = Linear;
   MagFilter = Nearest;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

sampler2D VortexSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Nearest;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

void p_DrawDensity
(
    in float2 vertexTCoord : TEXCOORD0,
    out float4 pixelColor : COLOR
)
{
    float4 state = tex2D(StateSampler, vertexTCoord);
    float4 color = tex1D(ColorTableSampler, state.w);
    float4 vortex = tex2D(VortexSampler, vertexTCoord);

    pixelColor.rgb = (1.0f-vortex.a)*color.rgb + vortex.a*vortex.rgb;
    pixelColor.a = 1.0f;
}
