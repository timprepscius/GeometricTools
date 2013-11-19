sampler2D DiffuseSampler = sampler_state
{
   MinFilter = Linear;
   MagFilter = Linear;
   WrapS     = Repeat;
   WrapT     = Repeat;
};

void v_DLitMatTex
(
    in float3 modelPosition : POSITION,
    in float3 modelNormal : TEXCOORD2,
    in float2 modelTCoord : TEXCOORD0,
    out float4 clipPosition : POSITION,
    out float3 vertexPosition : TEXCOORD0,
    out float3 vertexNormal : TEXCOORD1,
    out float2 vertexTCoord : TEXCOORD2,
    uniform float4x4 PVWMatrix
)
{
    clipPosition = mul(PVWMatrix, float4(modelPosition,1.0f));
    vertexPosition = modelPosition;
    vertexNormal = modelNormal;
    vertexTCoord = modelTCoord;
}

void p_DLitMatTex
(
    in float3 vertexPosition : TEXCOORD0,
    in float3 vertexNormal : TEXCOORD1,
    in float2 vertexTCoord : TEXCOORD2,
    out float4 pixelColor : COLOR,
    uniform float3 CameraModelPosition,
    uniform float3 MaterialAmbient,
    uniform float4 MaterialSpecular,
    uniform float3 LightModelDirection,
    uniform float3 LightColor
)
{
    // Compute the lighting factors.
    float3 normal = normalize(vertexNormal);
    float3 lightModelVector = normalize(LightModelDirection);
    float NDotL = -dot(normal, lightModelVector);
    float3 viewVector = normalize(CameraModelPosition - vertexPosition);
    float3 halfVector = normalize(viewVector - lightModelVector);
    float NDotH = dot(normal, halfVector);
    float4 lighting = lit(NDotL, NDotH, MaterialSpecular.a);

    // Diffuse color comes from the texture.
    float4 MaterialDiffuse = tex2D(DiffuseSampler, vertexTCoord);

    // Compute the pixel color.
    pixelColor.rgb =
        (MaterialAmbient + LightColor*lighting.y)*MaterialDiffuse.rgb +
        lighting.z*MaterialSpecular.rgb;

    pixelColor.a = MaterialDiffuse.a;
}
