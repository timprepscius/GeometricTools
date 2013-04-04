sampler3D BaseSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Linear;
   WrapS     = Clamp;
   WrapT     = Clamp;
   WrapR     = Clamp;
};

void v_VolumeTextures
(
    in float3 modelPosition : POSITION,
    in float3 modelTCoord : TEXCOORD0,
    out float4 clipPosition : POSITION,
    out float3 vertexTCoord : TEXCOORD0,
    uniform float4x4 PVWMatrix
)
{
    // Transform the position from model space to clip space.
    clipPosition = mul(PVWMatrix, float4(modelPosition,1.0f));

    // Pass through the texture coordinate.
    vertexTCoord = modelTCoord;
}

void p_VolumeTextures
(
    in float3 vertexTCoord : TEXCOORD0,
    out float4 pixelColor : COLOR,
    uniform sampler3D BaseSampler
)
{
    pixelColor = tex3D(BaseSampler, vertexTCoord);
}
