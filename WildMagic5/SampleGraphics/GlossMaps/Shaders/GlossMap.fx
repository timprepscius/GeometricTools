sampler2D BaseSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Nearest;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

void v_GlossMap
(
    in float3 modelPosition : POSITION,
    in float3 modelNormal : NORMAL,
    in float2 modelTCoord : TEXCOORD0,
    out float4 clipPosition : POSITION,
    out float3 emsAmbDiffColor : COLOR,
    out float3 specColor : TEXCOORD0,
    out float2 vertexTCoord : TEXCOORD1,
    uniform float4x4 PVWMatrix,
    uniform float3 CameraModelPosition,
    uniform float3 MaterialEmissive,
    uniform float3 MaterialAmbient,
    uniform float4 MaterialDiffuse,
    uniform float4 MaterialSpecular,
    uniform float3 LightModelDirection,
    uniform float3 LightAmbient,
    uniform float3 LightDiffuse,
    uniform float3 LightSpecular
)
{
    // Transform the position from model space to clip space.
    clipPosition = mul(PVWMatrix, float4(modelPosition,1.0f));

    // Compute the lighting factors.
    float NDotL = -dot(modelNormal, LightModelDirection);
    float3 viewVector = normalize(CameraModelPosition - modelPosition);
    float3 halfVector = normalize(viewVector - LightModelDirection);
    float NDotH = dot(modelNormal, halfVector);
    float4 lighting = lit(NDotL, NDotH, MaterialSpecular.a);

    emsAmbDiffColor = MaterialEmissive + MaterialAmbient*LightAmbient
        + lighting.y*MaterialDiffuse.rgb*LightDiffuse;
        
    specColor = lighting.z*MaterialSpecular.rgb*LightSpecular;

    // Pass through the texture coordinate.
    vertexTCoord = modelTCoord;
}

void p_GlossMap
(
    in float3 emsAmbDiffColor : COLOR,
    in float3 specColor : TEXCOORD0,
    in float2 vertexTCoord : TEXCOORD1,
    out float4 pixelColor : COLOR
)
{
    // The blending equation is
    //   (rf,gf,bf) = (rt,gt,bt)*(re,ge,be) + at*(rs,gs,bs)
    // where (rf,gf,bf) is the final color, (rt,gt,bt) is the texture color,
    // (re,ge,be) is the emissive-ambient-diffuse color, and (rs,gs,bs) is
    // the specular color.

    float4 baseColor = tex2D(BaseSampler, vertexTCoord);
    pixelColor.rgb = baseColor.rgb*emsAmbDiffColor + baseColor.a*specColor;
    pixelColor.a = 1.0f;
}
