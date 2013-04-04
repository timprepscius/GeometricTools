sampler2D ShadowSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Linear;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

void v_SMUnlit
(
    in float3 modelPosition : POSITION,
    out float4 clipPosition : POSITION,
    out float4 projTCoord : TEXCOORD0,
    out float depth : TEXCOORD1,
    uniform float4x4 PVWMatrix,
    uniform float4x4 WMatrix,
    uniform float4x4 LightPVMatrix,
    uniform float4x4 LightBSMatrix
)
{
    // Transform the position from model space to clip space.
    float4 hModelPosition = float4(modelPosition, 1.0f);
    clipPosition = mul(PVWMatrix, hModelPosition);

    // Transform the position from model space to light space.
    float4 worldPosition = mul(WMatrix, hModelPosition);
    float4 lightSpacePosition = mul(LightPVMatrix, worldPosition);

    // Compute the projected texture coordinates.
    projTCoord = mul(LightBSMatrix, lightSpacePosition);

    // Output the distance from the light source.
    depth = lightSpacePosition.z;
}

void p_SMUnlit
(
    in float4 projTCoord : TEXCOORD0,
    in float depth : TEXCOORD1,
    out float4 pixelColor : COLOR0,
    uniform float DepthBias,
    uniform float2 TexelSize  // (1/texWidth, 1/texHeight)
)
{
    // Generate the texture coordinates for the specified depth-map size.
    float4 tcoords[9];
    tcoords[0] = projTCoord;
    tcoords[1] = projTCoord + float4(-TexelSize.x, 0.0f,         0.0f, 0.0f);
    tcoords[2] = projTCoord + float4(+TexelSize.x, 0.0f,         0.0f, 0.0f);
    tcoords[3] = projTCoord + float4(0.0f,         -TexelSize.y, 0.0f, 0.0f);
    tcoords[4] = projTCoord + float4(-TexelSize.x, -TexelSize.y, 0.0f, 0.0f);
    tcoords[5] = projTCoord + float4(+TexelSize.x, -TexelSize.y, 0.0f, 0.0f);
    tcoords[6] = projTCoord + float4(0.0f,         +TexelSize.y, 0.0f, 0.0f);
    tcoords[7] = projTCoord + float4(-TexelSize.x, +TexelSize.y, 0.0f, 0.0f);
    tcoords[8] = projTCoord + float4(+TexelSize.x, +TexelSize.y, 0.0f, 0.0f);
    float w = projTCoord.w;

    // Sample each of them, checking whether or not the pixel is shadowed.
    float diff = depth - DepthBias;
    float shadowTerm = 0.0f;
    for (int i = 0; i < 9; ++i)
    {
        tcoords[i] /= w;
        float rvalue = tex2D(ShadowSampler, tcoords[i].xy).r;
        if (rvalue >= diff)
        {
            // The pixel is NOT in shadow.
            shadowTerm += 1.0f;
        }
    }
    shadowTerm /= 9.0f;
    pixelColor = shadowTerm;
}
