void v_Material
(
    in float3 modelPosition : POSITION,
    out float4 clipPosition : POSITION,
    out float4 vertexColor : COLOR,
    uniform float4x4 PVWMatrix,
    uniform float4 MaterialDiffuse)
{
    // Transform the position from model space to clip space.
    clipPosition = mul(PVWMatrix, float4(modelPosition,1.0f));

    // Use the material diffuse color as the vertex color.
    vertexColor = MaterialDiffuse;
}

void p_Material
(
    in float4 vertexColor : COLOR,
    out float4 pixelColor : COLOR)
{
    pixelColor = vertexColor;
}
