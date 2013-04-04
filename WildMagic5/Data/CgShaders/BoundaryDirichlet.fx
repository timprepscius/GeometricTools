sampler2D MaskSampler = sampler_state
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

void p_BoundaryDirichlet
(
    in float2 vertexTCoord : TEXCOORD0,
    out float4 pixelColor : COLOR
)
{
    float4 mask = tex2D(MaskSampler, vertexTCoord);
    float4 state = tex2D(StateSampler, vertexTCoord);
    pixelColor = mask*state;
}
