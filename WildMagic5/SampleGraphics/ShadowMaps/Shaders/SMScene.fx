sampler2D BaseSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Linear;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

sampler2D BlurSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Linear;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

sampler2D ProjectedSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Linear;
   WrapS     = Clamp;
   WrapT     = Clamp;
};

void v_SMScene
(
    in float3 modelPosition : POSITION,
    in float3 modelNormal : NORMAL,
    in float2 modelTCoord : TEXCOORD0,
    out float4 clipPosition : POSITION,
    out float2 vertexTCoord : TEXCOORD0,
    out float4 projTCoord : TEXCOORD1,
    out float4 screenTCoord : TEXCOORD2,
    out float3 vertexNormal : TEXCOORD3,
    out float3 lightVector : TEXCOORD4,
    out float3 eyeVector : TEXCOORD5,
    uniform float4x4 PVWMatrix,
    uniform float4x4 WMatrix,
    uniform float4x4 LightPVMatrix,
    uniform float4x4 LightBSMatrix,
    uniform float4x4 ScreenBSMatrix,
    uniform float3 LightWorldPosition,
    uniform float3 CameraWorldPosition
)
{
    // Transform the position from model space to clip space.
    float4 hModelPosition = float4(modelPosition, 1.0f);
    clipPosition = mul(PVWMatrix, hModelPosition);

    // Pass through the texture coordinates.
    vertexTCoord = modelTCoord;

    // Transform the position from model space to world space.
    float4 worldPosition = mul(WMatrix, hModelPosition);

    // Transform the normal from model space to world space.
    float4 hModelNormal = float4(modelNormal, 0.0f);
    vertexNormal = mul(WMatrix, hModelNormal).xyz;

    // Compute the projected texture coordinates.
    float4 lightSpacePosition = mul(LightPVMatrix, worldPosition);
    projTCoord = mul(LightBSMatrix, lightSpacePosition);

    // Compute the screen-space texture coordinates.
    screenTCoord = mul(ScreenBSMatrix, clipPosition);

    // Transform the light vector to tangent space.
    lightVector = LightWorldPosition - worldPosition.xyz;

    // Transform the eye vector into tangent space.
    eyeVector = CameraWorldPosition - worldPosition.xyz;
}

void p_SMScene
(
    in float2 vertexTCoord : TEXCOORD0,
    in float4 projTCoord : TEXCOORD1,
    in float4 screenTCoord : TEXCOORD2,
    in float3 vertexNormal : TEXCOORD3,
    in float3 lightVector : TEXCOORD4,
    in float3 eyeVector : TEXCOORD5,
    out float4 pixelColor : COLOR,
    uniform float4 LightColor
)
{
    // Normalize the input vectors.
    vertexNormal = normalize(vertexNormal);
    lightVector = normalize(lightVector);
    eyeVector = normalize(eyeVector);

    // Get the base color.
    float4 baseColor = tex2D(BaseSampler, vertexTCoord);

    // Compute the ambient lighting term (zero, for this example).
    float ambient = 0.0f;

    // Compute the diffuse lighting term.
    float NdL = dot(vertexNormal, lightVector);
    float diffuse = max(NdL, 0.0f);

    // Compute the specular lighting term.
    float specular;
    if (diffuse != 0.0f)
    {
        float3 tmp = 2.0f*NdL*vertexNormal - lightVector;
        specular = pow(max(dot(tmp, lightVector), 0.0f), 8.0f);
    }
    else
    {
        specular = 0.0f;
    }

    // Clamp the spot texture to a disk centered in the texture in
    // order to give the appearance of a spotlight cone.
    float u = projTCoord.x/projTCoord.w;
    float v = projTCoord.y/projTCoord.w;
    float du = u - 0.5f;
    float dv = v - 0.5f;
    float rsqr = du*du + dv*dv;
    float weight = (rsqr <= 0.25f ? 0.0f : 1.0f);

    float shadow = tex2Dproj(BlurSampler, screenTCoord).r;
    float4 projColor = tex2D(ProjectedSampler, float2(u, v));
    float4 shadowColor = shadow*projColor;
    float4 ambientColor = ambient*baseColor;
    float4 diffuseColor = diffuse*baseColor*LightColor;
    float4 specularColor = specular*baseColor*LightColor.a;

    pixelColor = ambientColor + (diffuseColor + specularColor)*(
        weight + (1.0f - weight)*shadowColor);

    pixelColor.a = 1.0f;
}
