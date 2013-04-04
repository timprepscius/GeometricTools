samplerCUBE CubeMapSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Linear;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

void v_CubeMap
(
    in float3 modelPosition : POSITION,
    in float3 modelNormal : NORMAL,
    in float3 modelColor : COLOR,
    out float4 clipPosition : POSITION,
    out float4 vertexColor : COLOR,
    out float3 cubeTCoord : TEXCOORD0,
    uniform float4x4 PVWMatrix,
    uniform float4x4 WMatrix,
    uniform float3 CameraWorldPosition
)
{
    // Transform the position from model space to clip space.
    float4 hModelPosition = float4(modelPosition, 1.0f);
    clipPosition = mul(PVWMatrix, hModelPosition);

    // Transform the position from model space to world space.
    float3 worldPosition = mul(WMatrix, hModelPosition).xyz;

    // Transform the normal from model space to world space.
    float4 hModelNormal = float4(modelNormal, 0.0f);
    float3 worldNormal = normalize(mul(WMatrix, hModelNormal)).xyz;

    // Calculate the eye direction.  The direction does not have to be
    // normalized, because the texture coordinates for the cube map are
    // invariant to scaling.  Thus, directions V and s*V for s > 0
    // generate the same texture coordinates.
    float3 eyeDirection = worldPosition - CameraWorldPosition;

    // Calculate the reflected vector.
    cubeTCoord = reflect(eyeDirection, worldNormal);

    // Pass through the model color.
    vertexColor.rgb = modelColor;
    vertexColor.a = 1.0f;
}

void p_CubeMap
(
    in float4 vertexColor : COLOR,
    in float3 cubeTCoord : TEXCOORD0,
    out float4 pixelColor : COLOR,
    uniform float Reflectivity
)
{
    float4 reflectedColor = texCUBE(CubeMapSampler, cubeTCoord);
    pixelColor = lerp(vertexColor, reflectedColor, Reflectivity);
}
