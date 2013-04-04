sampler2D BaseSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Linear;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

sampler2D DetailSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Linear;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

void v_BaseMulDetailFogExpSqr
(
    in float3 modelPosition : POSITION,
    in float2 modelBaseTCoord : TEXCOORD0,
    in float2 modelDetailTCoord : TEXCOORD1,
    out float4 clipPosition : POSITION,
    out float2 vertexBaseTCoord : TEXCOORD0,
    out float2 vertexDetailTCoord : TEXCOORD1,
    out float4 vertexFogInfo : TEXCOORD2,
    uniform float4x4 PVWMatrix,
    uniform float4x4 VWMatrix,
    uniform float4 FogColorDensity
)
{
    // Transform the position from model space to clip space.
    float4 hModelPosition = float4(modelPosition, 1.0f);
    clipPosition = mul(PVWMatrix, hModelPosition);

    // Transform the position from model space to view space.  This is the
    // vector from the view-space eye position (the origin) to the view-space
    // vertex position.  The fog factor (vertexFogInfo.w) uses the
    // z-component of this vector, which is z-based depth, not range-based
    // depth.
    float3 viewPosition = mul(VWMatrix, hModelPosition).xyz;
    float fogSqrDistance = dot(viewPosition, viewPosition);
    float fogExpArg = -FogColorDensity.w*FogColorDensity.w*fogSqrDistance;
    vertexFogInfo.rgb = FogColorDensity.rgb;
    vertexFogInfo.w = exp(fogExpArg);

    // Pass through the texture coordinates.
    vertexBaseTCoord = modelBaseTCoord;
    vertexDetailTCoord = modelDetailTCoord;
}

void p_BaseMulDetailFogExpSqr
(
    in float2 vertexBaseTCoord : TEXCOORD0,
    in float2 vertexDetailTCoord : TEXCOORD1,
    in float4 vertexFogInfo : TEXCOORD2,
    out float4 pixelColor : COLOR
)
{
    // Sample the texture images and multiply the results.
    float3 baseColor = tex2D(BaseSampler, vertexBaseTCoord).xyz;
    float3 detailColor = tex2D(DetailSampler, vertexDetailTCoord).xyz;
    float3 product = baseColor*detailColor;

    // Combine the base*detail color with the fog color.
    pixelColor.rgb = lerp(vertexFogInfo.rgb, product, vertexFogInfo.w);
    pixelColor.a = 1.0f;
}
