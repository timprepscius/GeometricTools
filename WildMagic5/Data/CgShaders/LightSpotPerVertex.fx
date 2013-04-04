void v_LightSpotPerVertex
(
    in float3 modelPosition  : POSITION,
    in float3 modelNormal : NORMAL,
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
    uniform float3 LightModelDirection,
    uniform float3 LightAmbient,
    uniform float3 LightDiffuse,
    uniform float3 LightSpecular,
    uniform float4 LightSpotCutoff,
    uniform float4 LightAttenuation
)
{
    // Transform the position from model space to clip space.
    clipPosition = mul(PVWMatrix, float4(modelPosition,1.0f));

    // Compute the lighting factors.
    float4 lighting;
    float3 modelLightDiff = modelPosition - LightModelPosition;
    float3 vertexDirection = normalize(modelLightDiff);
    float vertexCosAngle = dot(LightModelDirection, vertexDirection);
    if (vertexCosAngle >= LightSpotCutoff.y)
    {
        float NDotL = -dot(modelNormal, vertexDirection);
        float3 viewVector = normalize(CameraModelPosition - modelPosition);
        float3 halfVector = normalize(viewVector - vertexDirection);
        float NDotH = dot(modelNormal, halfVector);
        lighting = lit(NDotL, NDotH, MaterialSpecular.a);
        lighting.w = pow(vertexCosAngle, LightSpotCutoff.w);
    }
    else
    {
        lighting = float4(1.0f, 0.0f, 0.0f, 0.0f);
    }

    // Compute the distance-based attenuation.
    float3 worldLightDiff = mul((float3x3)WMatrix, modelLightDiff);
    float distance = length(worldLightDiff);
    float attenuation = LightAttenuation.w/(LightAttenuation.x + distance *
        (LightAttenuation.y + distance*LightAttenuation.z));

    // Compute the lighting color.
    float3 color = MaterialAmbient*LightAmbient + lighting.w*(
        lighting.y*MaterialDiffuse.rgb*LightDiffuse +
        lighting.z*MaterialSpecular.rgb*LightSpecular);
    
    // Compute the vertex color.
    vertexColor.rgb = MaterialEmissive + attenuation*color;
    vertexColor.a = MaterialDiffuse.a;
}

void p_LightSpotPerVertex
(
    in float4 vertexColor : COLOR,
    out float4 pixelColor : COLOR
)
{
    pixelColor = vertexColor;
}
