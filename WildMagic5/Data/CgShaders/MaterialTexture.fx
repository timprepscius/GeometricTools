// The application can changes these parameters at run time.
sampler2D BaseSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Nearest;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

void v_MaterialTexture
(
    in float3 modelPosition : POSITION,
    in float2 modelTCoord : TEXCOORD0,
    out float4 clipPosition : POSITION,
    out float4 vertexColor : COLOR,
    out float2 vertexTCoord : TEXCOORD0,
    uniform float4x4 PVWMatrix,
    uniform float4 MaterialDiffuse)
   
{
    // Transform the position from model space to clip space.
    clipPosition = mul(PVWMatrix, float4(modelPosition,1.0f));

    // Pass through the material diffuse color.
    vertexColor = MaterialDiffuse;

    // Pass through the texture coordinate.
    vertexTCoord = modelTCoord;
}

void p_MaterialTexture
(
    in float4 vertexColor : COLOR,
    in float2 vertexTCoord : TEXCOORD0,
    out float4 pixelColor : COLOR
)
{
    // Add the material and texture colors.
    float4 baseColor = tex2D(BaseSampler, vertexTCoord);
    pixelColor.rgb = saturate(baseColor.rgb + vertexColor.rgb);

    // Multiply the material and texture alphas.
    pixelColor.a = baseColor.a*vertexColor.a;
}
