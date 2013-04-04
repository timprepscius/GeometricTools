sampler2D StateSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Nearest;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

sampler2D MaskSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Nearest;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

void v_ScreenShader
(
    in float3 modelPosition : POSITION,
    in float2 modelTCoord : TEXCOORD0,
    out float4 clipPosition : POSITION,
    out float2 vertexTCoord : TEXCOORD0,
    uniform float4x4 PVWMatrix
)
{
    // Transform the position from model space to clip space.
    clipPosition = mul(PVWMatrix, float4(modelPosition,1.0f));

    // Pass through the texture coordinates.
    vertexTCoord = modelTCoord;
}

void p_ScreenShader2
(
    in float2 vertexTCoord : TEXCOORD0,
    out float4 pixelColor : COLOR
)
{
    pixelColor = tex2D(StateSampler, vertexTCoord);
}

void p_ScreenShader3
(
    in float2 vertexTCoord : TEXCOORD0,
    uniform float4 BoundaryColor,
    out float4 pixelColor : COLOR
)
{
    float4 state = tex2D(StateSampler, vertexTCoord);
    float4 mask = tex2D(MaskSampler, vertexTCoord);
    float4 one = float4(1.0f, 1.0f, 1.0f, 1.0f);
    pixelColor = mask*state + (one - mask)*BoundaryColor;
}
