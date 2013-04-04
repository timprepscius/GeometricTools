void v_SMShadow
(
    in float3 modelPosition : POSITION,
    out float4 lightSpacePosition : POSITION,
    out float depth : TEXCOORD0,
    uniform float4x4 WMatrix,
    uniform float4x4 LightPVMatrix
)
{
    // Transform the position from model space to light space.
    float4 worldPosition = mul(WMatrix, float4(modelPosition, 1.0f));
    lightSpacePosition = mul(LightPVMatrix, worldPosition);

    // Output the distance from the light source to the vertex.
    depth = lightSpacePosition.z;
}

void p_SMShadow
(
    in float depth : TEXCOORD0,
    out float4 pixelColor : COLOR0
)
{
    pixelColor = depth;
}
