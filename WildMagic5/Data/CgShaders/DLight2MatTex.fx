sampler2D BaseSampler = sampler_state
{
   MinFilter = Linear;
   MagFilter = Linear;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

void v_DLight2MatTex
(
    in float3 modelPosition : POSITION,
    in float3 modelNormal : NORMAL,
    in float2 modelTCoord : TEXCOORD0,
    out float4 clipPosition : POSITION,
    out float4 vertexColor : COLOR,
    out float2 vertexTCoord : TEXCOORD0,
    uniform float4x4 PVWMatrix,
    uniform float3 CameraModelPosition,
    uniform float3 MaterialEmissive,
    uniform float3 MaterialAmbient,
    uniform float4 MaterialDiffuse,
    uniform float4 MaterialSpecular,
    uniform float3 Light0ModelDirection,
    uniform float3 Light0Ambient,
    uniform float3 Light0Diffuse,
    uniform float3 Light0Specular,
    uniform float4 Light0Attenuation,
    uniform float3 Light1ModelDirection,
    uniform float3 Light1Ambient,
    uniform float3 Light1Diffuse,
    uniform float3 Light1Specular,
    uniform float4 Light1Attenuation
)
{
    // Transform the position from model space to clip space.
    clipPosition = mul(PVWMatrix, float4(modelPosition, 1.0f));

    // Compute the lighting factors.
    float3 viewVector = normalize(CameraModelPosition - modelPosition);

    float NDotL0 = -dot(modelNormal, Light0ModelDirection);
    float3 halfVector0 = normalize(viewVector - Light0ModelDirection);
    float NDotH0 = dot(modelNormal, halfVector0);
    float4 lighting0 = lit(NDotL0, NDotH0, MaterialSpecular.a);

    float NDotL1 = -dot(modelNormal, Light1ModelDirection);
    float3 halfVector1 = normalize(viewVector - Light1ModelDirection);
    float NDotH1 = dot(modelNormal, halfVector1);
    float4 lighting1 = lit(NDotL1, NDotH1, MaterialSpecular.a);

    // Compute the lighting color.
    float3 color0 = MaterialAmbient*Light0Ambient
        + lighting0.y*MaterialDiffuse.rgb*Light0Diffuse
        + lighting0.z*MaterialSpecular.rgb*Light0Specular;

    float3 color1 = MaterialAmbient*Light1Ambient
        + lighting1.y*MaterialDiffuse.rgb*Light1Diffuse
        + lighting1.z*MaterialSpecular.rgb*Light1Specular;

    // Compute the vertex color.
    vertexColor.rgb = MaterialEmissive + Light0Attenuation.w*color0 +
        Light1Attenuation.w*color1;
    vertexColor.a = MaterialDiffuse.a;
    
    // Pass through the texture coordinate.
    vertexTCoord = modelTCoord;
}

void p_DLight2MatTex
(
    in float4 vertexColor : COLOR,
    in float2 vertexTCoord : TEXCOORD0,
    out float4 pixelColor : COLOR
)
{
    // Modulate the vertex and texture colors.
    float4 baseColor = tex2D(BaseSampler, vertexTCoord);
    pixelColor = baseColor*vertexColor;
}
