sampler2D BaseSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Linear;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

void v_VolumeFog
(
    in float3 modelPosition : POSITION,
    in float4 modelColor : COLOR,
    in float2 modelTCoord : TEXCOORD0,
    out float4 clipPosition : POSITION,
    out float4 vertexColor : COLOR,
    out float2 vertexTCoord : TEXCOORD0,
    uniform float4x4 PVWMatrix
)
{
    // Transform the position from model space to clip space.
    clipPosition = mul(PVWMatrix, float4(modelPosition,1.0f));

    // Pass through the vertex color and texture coordinate.
    vertexColor = modelColor;
    vertexTCoord = modelTCoord;
}

void p_VolumeFog
(
    in float4 vertexColor : COLOR,
    in float2 vertexTCoord : TEXCOORD0,
    out float4 pixelColor : COLOR
)
{
    // The blending equation is
    //   (rf,gf,bf) = (1-av)*(rt,gt,bt) + av*(rv,gv,bv)
    // where (rf,gf,bf) is the final color, (rt,gt,bt) is the texture color,
    // and (rv,gv,bv,av) is the vertex color.

    // Sample the texture image.
    float4 textureColor = tex2D(BaseSampler, vertexTCoord);
    pixelColor.rgb = (1.0f - vertexColor.a)*textureColor.rgb +
        vertexColor.a*vertexColor.rgb;
    pixelColor.a = 1.0f;
}
