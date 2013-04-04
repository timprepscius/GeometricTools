void v_Skinning
(
    in float3 modelPosition : POSITION,
    in float3 modelColor : COLOR0,
    in float4 modelWeights : TEXCOORD0,
    out float4 clipPosition : POSITION,
    out float3 vertexColor : COLOR0,
    uniform float4x4 PVWMatrix,
    uniform float4x4 SkinningMatrix0,
    uniform float4x4 SkinningMatrix1,
    uniform float4x4 SkinningMatrix2,
    uniform float4x4 SkinningMatrix3
)
{
    // This skinning shader has a fixed number of skinning matrices per
    // vertex.  If you want a number that varies with the vertex, pass in
    // an array of skinning matrices.  Also pass in texture coordinates
    // that are used as lookups into the array.

    // Calculate the position by adding together a convex combination of
    // transformed positions.
    float4 hModelPosition = float4(modelPosition, 1.0f);
    float4 position0 = mul(SkinningMatrix0, hModelPosition)*modelWeights.x;
    float4 position1 = mul(SkinningMatrix1, hModelPosition)*modelWeights.y;
    float4 position2 = mul(SkinningMatrix2, hModelPosition)*modelWeights.z;
    float4 position3 = mul(SkinningMatrix3, hModelPosition)*modelWeights.w;
    float4 skinPosition = position0 + position1 + position2 + position3;

    // Transform the position from model space to clip space.
    clipPosition = mul(PVWMatrix, skinPosition);

    // Pass through the color.
    vertexColor = modelColor;
}

void p_Skinning
(
    in float3 vertexColor : COLOR0,
    out float4 pixelColor : COLOR0
)
{
    pixelColor.rgb = vertexColor;
    pixelColor.a = 1.0f;
}
