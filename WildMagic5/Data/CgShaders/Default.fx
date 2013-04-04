void v_Default
(
    in float3 modelPosition : POSITION,
    out float4 clipPosition : POSITION,
    uniform float4x4 PVWMatrix
)
{
    // Transform the position from model space to clip space.
    clipPosition = mul(PVWMatrix, float4(modelPosition, 1.0f));
}

void p_Default
(
    out float4 pixelColor : COLOR
)
{
    // Return magenta.
    pixelColor = float4(1.0f, 0.0f, 1.0f, 1.0f);
}
