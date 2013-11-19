void v_LightPointPerVertex
(
    in float3 modelPosition : POSITION,
    in float3 modelNormal : TEXCOORD1,
    out float4 clipPosition : POSITION,
    out float4 vertexColor : COLOR,
    uniform float4x4 PVWMatrix,
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
    // Transform the position from model space to clip space.
    clipPosition = mul(PVWMatrix, float4(modelPosition,1.0f));

    // Compute the lighting factors.
    float3 modelLightDiff = modelPosition - LightModelPosition;
    float3 vertexDirection = normalize(modelLightDiff);
    float NDotL = -dot(modelNormal, vertexDirection);
    float3 viewVector = normalize(CameraModelPosition - modelPosition);
    float3 halfVector = normalize(viewVector - vertexDirection);
    float NDotH = dot(modelNormal, halfVector);
    float4 lighting = lit(NDotL, NDotH, MaterialSpecular.a);

    // Compute the distance-based attenuation.
    float3 worldLightDiff = mul((float3x3)WMatrix, modelLightDiff);
    float distance = length(worldLightDiff);
    float attenuation = LightAttenuation.w/(LightAttenuation.x + distance *
        (LightAttenuation.y + distance*LightAttenuation.z));

    // Compute the lighting color.
    float3 color = MaterialAmbient*LightAmbient
        + lighting.y*MaterialDiffuse.rgb*LightDiffuse
        + lighting.z*MaterialSpecular.rgb*LightSpecular;

    // Compute the vertex color.
    vertexColor.rgb = MaterialEmissive + attenuation*color;
    vertexColor.a = MaterialDiffuse.a;
}

void p_LightPointPerVertex
(
    in float4 vertexColor : COLOR,
    out float4 pixelColor : COLOR
)
{
    pixelColor = vertexColor;
}
