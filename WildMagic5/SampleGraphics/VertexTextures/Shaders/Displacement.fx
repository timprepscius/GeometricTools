sampler2D HeightSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Linear;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

void v_Displacement
(
    in float3 modelPosition : POSITION,
    in float2 modelTCoord0 : TEXCOORD0,
    out float4 clipPosition : POSITION,
    out float vertexHeight : TEXCOORD0,
    uniform float4x4 PVWMatrix
)
{
    float4 displacedPosition;
    displacedPosition.xy = modelPosition.xy;
    displacedPosition.z = tex2D(HeightSampler, modelTCoord0).r;
    displacedPosition.w = 1.0f;
    clipPosition = mul(PVWMatrix, displacedPosition);

    vertexHeight = displacedPosition.z;
}

void p_Displacement
(
    in float vertexHeight : TEXCOORD0,
    out float4 pixelColor : COLOR
)
{
    pixelColor.rgb = vertexHeight;
    pixelColor.a = 1.0f;
}
