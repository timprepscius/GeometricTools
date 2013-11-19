sampler2D ProjectorSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Linear;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

void v_ProjectedTexture
(
    in float3 modelPosition : POSITION,
    in float3 modelNormal : TEXCOORD1,
    out float4 clipPosition : POSITION,
    out float4 vertexColor : COLOR,
    out float4 vertexTCoord : TEXCOORD0,
    uniform float4x4 PVWMatrix,
    uniform float4x4 ProjectorMatrix,
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

    // Compute the lighting color.
    vertexColor.rgb = MaterialEmissive + MaterialAmbient*LightAmbient
        + lighting.y*MaterialDiffuse.rgb*LightDiffuse
        + lighting.z*MaterialSpecular.rgb*LightSpecular;
    vertexColor.a = MaterialDiffuse.a;
    
    // Compute the projected texture coordinates.
    vertexTCoord = mul(ProjectorMatrix, float4(modelPosition,1.0f));
}

void p_ProjectedTexture
(
    in float4 vertexColor : COLOR,
    in float4 vertexTCoord : TEXCOORD0,
    out float4 pixelColor : COLOR
)
{
    float4 projectorColor = tex2Dproj(ProjectorSampler, vertexTCoord);
    pixelColor = projectorColor*vertexColor;
}
