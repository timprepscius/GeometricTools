void v_LightAmbient
(
    in float3 modelPosition : POSITION,
    out float4 clipPosition : POSITION,
    out float4 vertexColor : COLOR,
    uniform float4x4 PVWMatrix,
    uniform float3 MaterialEmissive,
    uniform float3 MaterialAmbient,
    uniform float3 LightAmbient,
    uniform float4 LightAttenuation
)
{
    // Transform the position from model space to clip space.
    clipPosition = mul(PVWMatrix, float4(modelPosition,1.0f));

    float3 ambient = LightAttenuation.w*LightAmbient;
    vertexColor.rgb = MaterialEmissive + MaterialAmbient*ambient;
    vertexColor.a = 1.0f;
}

void p_LightAmbient
(
    in float4 vertexColor : COLOR,
    out float4 pixelColor : COLOR
)
{
    pixelColor = vertexColor;
}
