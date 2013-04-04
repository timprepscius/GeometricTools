// The application can changes these parameters at run time.
sampler2D Sampler0 = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Nearest;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

sampler2D Sampler1 = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Nearest;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

void v_MultipleRenderTarget
(
    in float3 modelPosition : POSITION,
    in float2 modelTCoord0 : TEXCOORD0,
    in float2 modelTCoord1 : TEXCOORD1,
    out float4 clipPosition : POSITION,
    out float2 vertexTCoord0 : TEXCOORD0,
    out float2 vertexTCoord1 : TEXCOORD1,
    uniform float4x4 PVWMatrix
)
{
    // Transform the position from model space to clip space.
    clipPosition = mul(PVWMatrix, float4(modelPosition,1.0f));

    // Pass through the texture coordinates.
    vertexTCoord0 = modelTCoord0;
    vertexTCoord1 = modelTCoord1;
}

void p_MultipleRenderTarget
(
    in float2 vertexTCoord0 : TEXCOORD0,
    in float2 vertexTCoord1 : TEXCOORD1,
    out float4 pixelColor0 : COLOR0,
    out float4 pixelColor1 : COLOR1
)
{
    // Sample the texture images and add the results.
    float4 color0 = tex2D(Sampler0, vertexTCoord0);
    float4 color1 = tex2D(Sampler1, vertexTCoord1);

    // Render target 0 gets the additive color.
    pixelColor0 = saturate(color0 + color1);

    // Render target 1 gets the green channel of the additive color.
    pixelColor1.g = pixelColor0.g;
    pixelColor1.rb = 0.0f;
    pixelColor1.a = 1.0f;
}
