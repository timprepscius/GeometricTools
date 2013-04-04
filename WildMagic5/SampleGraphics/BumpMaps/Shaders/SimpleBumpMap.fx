sampler2D BaseSampler = sampler_state
{
   MinFilter = Linear;
   MagFilter = Linear;
   WrapS     = Repeat;
   WrapT     = Repeat;
};

sampler2D NormalSampler = sampler_state
{
   MinFilter = Linear;
   MagFilter = Linear;
   WrapS     = Repeat;
   WrapT     = Repeat;
};

void v_SimpleBumpMap
(
    in float3 modelPosition : POSITION,
    in float3 modelLightDirection : COLOR0,
    in float2 modelBaseTCoord : TEXCOORD0,
    in float2 modelNormalTCoord : TEXCOORD1,
    out float4 clipPosition : POSITION,
    out float3 vertexLightDirection : COLOR0,
    out float2 vertexBaseTCoord : TEXCOORD0,
    out float2 vertexNormalTCoord : TEXCOORD1,
    uniform float4x4 PVWMatrix
)
{
    // Transform the position from model space to clip space.
    clipPosition = mul(PVWMatrix, float4(modelPosition,1.0f));

    // Pass through the parameters.
    vertexBaseTCoord = modelBaseTCoord;
    vertexNormalTCoord = modelNormalTCoord;
    vertexLightDirection = modelLightDirection;
}

void p_SimpleBumpMap
(
    in float3 vertexLightDirection : COLOR0,
    in float2 vertexBaseTCoord : TEXCOORD0,
    in float2 vertexNormalTCoord : TEXCOORD1,
    out float4 pixelColor : COLOR
)
{
    float3 baseColor = tex2D(BaseSampler, vertexBaseTCoord).rgb;
    float3 normalColor = tex2D(NormalSampler, vertexNormalTCoord).rgb;

    // The lack of normalizations of the light and normal vectors
    // allows this to compile for Shader Model 1 (ps_1_1).
    float3 lightDirection = 2.0f*vertexLightDirection - 1.0f;
    float3 normalDirection = 2.0f*normalColor - 1.0f;
    float LdN = saturate(dot(lightDirection, normalDirection));

    pixelColor.rgb = LdN*baseColor;
    pixelColor.a = 1.0f;
}
