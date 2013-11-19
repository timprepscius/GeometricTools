void v_LightPointPerPixel
(
    in float3 modelPosition : POSITION,
    in float3 modelNormal : TEXCOORD1,
    out float4 clipPosition : POSITION,
    out float3 vertexPosition : TEXCOORD0,
    out float3 vertexNormal : TEXCOORD1,
    uniform float4x4 PVWMatrix
)
{
    clipPosition = mul(PVWMatrix, float4(modelPosition,1.0f));
    vertexPosition = modelPosition;
    vertexNormal = modelNormal;
}

void p_LightPointPerPixel
(
    in float3 vertexPosition : TEXCOORD0,
    in float3 vertexNormal : TEXCOORD1,
    out float4 pixelColor : COLOR,
    uniform float4x4 WMatrix,
    uniform float3 CameraModelPosition,
    uniform float3 MaterialEmissive,
    uniform float3 MaterialAmbient,
    uniform float4 MaterialDiffuse,
    uniform float4 MaterialSpecular,
    uniform float3 LightModelPosition,
    uniform float3 LightAmbient,
    uniform float3 LightDiffuse,
    uniform float3 LightSpecular,
    uniform float4 LightAttenuation
)
{
    // Compute the lighting factors.
    float3 normal = normalize(vertexNormal);
    float3 vertexLightDiff = vertexPosition - LightModelPosition;
    float3 vertexDirection = normalize(vertexLightDiff);
    float NDotL = -dot(normal, vertexDirection);
    float3 viewVector = normalize(CameraModelPosition - vertexPosition);
    float3 halfVector = normalize(viewVector - vertexDirection);
    float NDotH = dot(normal, halfVector);
    float4 lighting = lit(NDotL, NDotH, MaterialSpecular.a);

    // Compute the distance-based attenuation.
    float3 worldLightDiff = mul((float3x3)WMatrix, vertexLightDiff);
    float distance = length(worldLightDiff);
    float attenuation = LightAttenuation.w/(LightAttenuation.x + distance *
        (LightAttenuation.y + distance*LightAttenuation.z));

    // Compute the lighting color.
    float3 color = MaterialAmbient*LightAmbient
        + lighting.y*MaterialDiffuse.rgb*LightDiffuse
        + lighting.z*MaterialSpecular.rgb*LightSpecular;

    // Compute the pixel color.
    pixelColor.rgb = MaterialEmissive + attenuation*color;
    pixelColor.a = MaterialDiffuse.a;
}
