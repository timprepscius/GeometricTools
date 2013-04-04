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

void p_GaussianBlur3
(
    in float2 vertexTCoord : TEXCOORD0,
    out float4 pixelColor : COLOR,
    uniform float4 Delta,  // (dx, 0, 0, dy)
    uniform float4 Weight  // (wx, wy, wz, ww = 1-2*wx-2*wy-2*wz)
)
{
    // vertexTCoord is the location of S(x,y,z)
    // vertexTCoord + Delta.xy is the location of S(x+1,y,z)
    // vertexTCoord - Delta.xy is the location of S(x-1,y,z)
    // vertexTCoord + Delta.zw is the location of S(x,y+1,z)
    // vertexTCoord - Delta.zw is the location of S(x,y-1,z)
    // vertexTCoord + offset.xy is the location of S(x,y,z+1)
    // vertexTCoord + offset.zw is the location of S(x,y,z-1)

    float4 offset = tex2D(OffsetSampler, vertexTCoord);

    float4 cZZZ = tex2D(StateSampler, vertexTCoord);
    float4 cPZZ = tex2D(StateSampler, vertexTCoord + Delta.xy);
    float4 cMZZ = tex2D(StateSampler, vertexTCoord - Delta.xy);
    float4 cZPZ = tex2D(StateSampler, vertexTCoord + Delta.zw);
    float4 cZMZ = tex2D(StateSampler, vertexTCoord - Delta.zw);
    float4 cZZP = tex2D(StateSampler, vertexTCoord + offset.xy);
    float4 cZZM = tex2D(StateSampler, vertexTCoord + offset.zw);

    pixelColor =
      Weight.x*(cPZZ + cMZZ) +
      Weight.y*(cZPZ + cZMZ) +
      Weight.z*(cZZP + cZZM) +
      Weight.w*cZZZ;
}
