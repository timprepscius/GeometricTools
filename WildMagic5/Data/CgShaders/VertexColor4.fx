void v_VertexColor4
(
    in float3 modelPosition : POSITION,
    in float4 modelColor : COLOR,
    out float4 clipPosition : POSITION,
    out float4 vertexColor : COLOR,
    uniform float4x4 PVWMatrix
)
{
    // Transform the position from model space to clip space.
    clipPosition = mul(PVWMatrix, float4(modelPosition,1.0f));

    // Pass through the color.
    vertexColor = modelColor;
}

void p_VertexColor4
(
    in float4 vertexColor : COLOR,
    out float4 pixelColor : COLOR
)
{
    // Pass through the color.
    pixelColor = vertexColor;
}
