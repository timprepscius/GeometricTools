// The application can changes these parameters at run time.
sampler2D BaseSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Nearest;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

void v_VertexColor4Texture
(
    in float3 modelPosition : POSITION,
    in float2 modelTCoord : TEXCOORD0,
    in float4 modelColor : COLOR,
    out float4 clipPosition : POSITION,
    out float2 vertexTCoord : TEXCOORD0,
    out float4 vertexColor : COLOR,
    uniform float4x4 PVWMatrix
)
{
    // Transform the position from model space to clip space.
    clipPosition = mul(PVWMatrix, float4(modelPosition,1.0f));

    // Pass through the texture coordinate.
    vertexTCoord = modelTCoord;

    // Pass through the vertex color.
    vertexColor = modelColor;
}

void p_VertexColor4Texture
(
    in float2 vertexTCoord : TEXCOORD0,
    in float4 vertexColor : COLOR,
    out float4 pixelColor : COLOR
)
{
    // Add the vertex and texture colors.
    float4 baseColor = tex2D(BaseSampler, vertexTCoord);
    pixelColor.rgb = saturate(baseColor.rgb + vertexColor.rgb);

    // Multiply the vertex and texture alphas.
    pixelColor.a = baseColor.a*vertexColor.a;
}
