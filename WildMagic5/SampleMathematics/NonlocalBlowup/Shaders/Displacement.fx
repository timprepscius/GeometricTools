sampler2D HeightSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Linear;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

sampler2D GridSampler = sampler_state
{
   MinFilter = Linear;
   MagFilter = Linear;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

sampler1D ColorSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Linear;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

sampler2D DomainSampler = sampler_state
{
   MinFilter = Linear;
   MagFilter = Linear;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

void v_Displacement
(
    in float3 modelPosition : POSITION,
    in float2 modelTCoord0 : TEXCOORD0,
    out float4 clipPosition : POSITION,
    out float3 vertexTCoord0 : TEXCOORD0,
    uniform float4x4 PVWMatrix
)
{
    float4 displacedPosition;
    displacedPosition.xy = modelPosition.xy;
    displacedPosition.z = tex2D(HeightSampler, modelTCoord0).r;
    displacedPosition.w = 1.0f;
    clipPosition = mul(PVWMatrix, displacedPosition);

    vertexTCoord0.xy = modelTCoord0;
    vertexTCoord0.z = displacedPosition.z;
}

void p_Displacement
(
    in float3 vertexTCoord0 : TEXCOORD0,
    out float4 pixelColor : COLOR
)
{
    float4 gridColor = tex2D(GridSampler, vertexTCoord0.xy);
    float index = saturate(vertexTCoord0.z/16.0f);
    float4 color = tex1D(ColorSampler, index);
    float4 domainColor = tex2D(DomainSampler, vertexTCoord0.xy);
    pixelColor = domainColor*gridColor*color +
        (1.0f - domainColor)*float4(0.4f, 0.5f, 0.6f, 1.0f);
}
