// Replace this by your own function.  The application should implement
// 'float MyFunction (float)' in exactly the same way.
float MyFunction (float x)
{
    return (x - 1.1f)*(x + 2.2f);
}

#ifdef WM5_USE_DX9
sampler2D TrailingSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Nearest;
   WrapS     = Clamp;
   WrapT     = Clamp;
};
#endif

void v_RootFinder
(
    in float3 modelPosition : POSITION,
    in float2 modelTCoord : TEXCOORD0,
    out float4 clipPosition : POSITION,
    out float2 vertexTCoord : TEXCOORD0,
    uniform float4x4 PVWMatrix
)
{
    clipPosition = mul(PVWMatrix, float4(modelPosition, 1.0f));
    vertexTCoord = modelTCoord;
}

void p_RootFinder
(
    in float2 vertexTCoord : TEXCOORD0,
    out float4 pixelColor0 : COLOR0
)
{
#ifdef WM5_USE_DX9
    float2 unit = tex2D(TrailingSampler, vertexTCoord).rg;
#endif
#ifdef WM5_USE_OPENGL
    float ix = 4096.0f*vertexTCoord.x - 0.5f;
    float iy = 2048.0f*vertexTCoord.y - 0.5f;
    float i = ix + 4096.0f*iy;  // 0 <= i <= 2^{23}-1
    float2 unit = float2(1.0f + ldexp(i, -23), 1.0f + ldexp(i+1, -23));
#endif

    float quantum = 1.0f/255.0f, update;
    float number0, number1, function0, function1;

    pixelColor0 = float4(1.0f, 0.0f, 1.0f, 0.0f);
    for (int biased = 1; biased < 255; ++biased)
    {
        float unbiased = biased - 127.0f;
        number0 = ldexp(unit.x, unbiased);
        number1 = ldexp(unit.y, unbiased);
        function0 = MyFunction(number0);
        function1 = MyFunction(number1);
        if (sign(function0) * sign(function1) <= 0.0f)
        {
            update = biased*quantum;
            pixelColor0.r = min(update, pixelColor0.r);
            pixelColor0.g = max(update, pixelColor0.g);
        }

        function0 = MyFunction(-number0);
        function1 = MyFunction(-number1);
        if (sign(function0) * sign(function1) <= 0.0f)
        {
            update = biased*quantum;
            pixelColor0.b = min(update, pixelColor0.b);
            pixelColor0.a = max(update, pixelColor0.a);
        }
    }
}
