sampler2D BaseSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Linear;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

void v_SphereMap
(
    in float3 modelPosition : POSITION,
    in float3 modelNormal : TEXCOORD1,
    out float4 clipPosition : POSITION,
    out float2 vertexTCoord : TEXCOORD0,
    uniform float4x4 PVWMatrix,
    uniform float4x4 VWMatrix
)
{
    // Transform the position from model space to clip space.
    float4 hModelPosition = float4(modelPosition, 1.0f);
    clipPosition = mul(PVWMatrix, hModelPosition);

    // Transform the normal from model space to camera space.
    float4 hModelNormal = float4(modelNormal, 0.0f);
    float3 cameraSpaceNormal = normalize(mul(VWMatrix, hModelNormal).xyz);

    // Calculate the eye direction in camera space.
    float4 cameraSpacePosition = mul(VWMatrix, hModelPosition);
    float3 eyeDirection = normalize(cameraSpacePosition.xyz);

    // Calculate the reflection vector.
    float3 reflection = reflect(eyeDirection, cameraSpaceNormal);

    // Calculate the texture coordinates.
    float oneMRZ = 1.0f - reflection.z;
    float invLength = 1.0f/sqrt(reflection.x*reflection.x +
        reflection.y*reflection.y + oneMRZ*oneMRZ);
    vertexTCoord = 0.5f*(reflection.xy*invLength + 1.0f);
}

void p_SphereMap
(
    in float2 vertexTCoord : TEXCOORD0, 
    out float4 pixelColor : COLOR
)
{
    pixelColor = tex2D(BaseSampler, vertexTCoord);
}
