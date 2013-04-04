void v_VertexColor3
(
    in float3 modelPosition : POSITION,
    in float3 modelColor : COLOR,
    out float4 clipPosition : POSITION,
    out float3 vertexColor : COLOR,
    uniform float4x4 PVWMatrix
)
{
    // Transform the position from model space to clip space.
    clipPosition = mul(PVWMatrix, float4(modelPosition,1.0f));

    // Pass through the color.
    vertexColor = modelColor;
}

void p_VertexColor3
(
    in float3 vertexColor : COLOR,
    out float4 pixelColor : COLOR
)
{
    // Pass through the color.
    pixelColor.rgb = vertexColor.rgb;
    pixelColor.a = 1.0f;
}
