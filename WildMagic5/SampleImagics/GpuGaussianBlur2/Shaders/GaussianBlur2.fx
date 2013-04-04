sampler2D StateSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Nearest;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

void p_GaussianBlur2
(
    in float2 vertexTCoord : TEXCOORD0,
    out float4 pixelColor : COLOR,
    uniform float2 Delta,  // (dx, dy)
    uniform float3 Weight  // (wx, wy, wz = 1-2*wx-2*wy)
)
{
    float4 cZZ = tex2D(StateSampler, vertexTCoord);
    
    float4 cPZ = tex2D(StateSampler,
        float2(vertexTCoord.x + Delta.x, vertexTCoord.y));

    float4 cMZ = tex2D(StateSampler,
        float2(vertexTCoord.x - Delta.x, vertexTCoord.y));

    float4 cZP = tex2D(StateSampler,
        float2(vertexTCoord.x, vertexTCoord.y + Delta.y));

    float4 cZM = tex2D(StateSampler,
        float2(vertexTCoord.x, vertexTCoord.y - Delta.y));

    pixelColor = Weight.z*cZZ + Weight.x*(cPZ + cMZ) + Weight.y*(cZP + cZM);
}
