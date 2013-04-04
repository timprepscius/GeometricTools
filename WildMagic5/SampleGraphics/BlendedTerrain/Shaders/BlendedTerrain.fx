sampler2D GrassSampler = sampler_state
{
   MinFilter = Linear;
   MagFilter = Linear;
   WrapS     = Repeat;
   WrapT     = Repeat;
};

sampler2D StoneSampler = sampler_state
{
   MinFilter = Linear;
   MagFilter = Linear;
   WrapS     = Repeat;
   WrapT     = Repeat;
};

sampler1D BlendSampler = sampler_state
{
   MinFilter = Nearest;
   MagFilter = Linear;
   WrapS     = Repeat;
};

sampler2D CloudSampler = sampler_state
{
   MinFilter = Linear;
   MagFilter = Linear;
   WrapS     = Repeat;
   WrapT     = Repeat;
};

void v_BlendedTerrain
(
    in float3 modelPosition : POSITION,
    in float2 modelGroundTCoord : TEXCOORD0,
    in float modelBlendTCoord : TEXCOORD1,
    in float2 modelCloudTCoord : TEXCOORD2,
    out float4 clipPosition : POSITION,
    out float2 vertexGroundTCoord : TEXCOORD0,
    out float vertexBlendTCoord : TEXCOORD1,
    out float2 vertexCloudTCoord : TEXCOORD2,
    out float2 vertexFlowDirection : TEXCOORD3,
    uniform float4x4 PVWMatrix,
    uniform float2 FlowDirection)
{
    // Transform the position from model space to clip space.
    clipPosition = mul(PVWMatrix, float4(modelPosition,1.0f));

    // Pass through the texture coordinates.
    vertexGroundTCoord = modelGroundTCoord;
    vertexBlendTCoord = modelBlendTCoord;
    vertexCloudTCoord = modelCloudTCoord;

    // Pass through the flow direction, to be used as an offset in the pixel
    // program.
    vertexFlowDirection = FlowDirection;
}

void p_BlendedTerrain
(
    in float2 vertexGroundTCoord : TEXCOORD0,
    in float vertexBlendTCoord : TEXCOORD1,
    in float2 vertexCloudTCoord : TEXCOORD2,
    in float2 vertexFlowDirection : TEXCOORD3,
    out float4 pixelColor : COLOR,
    uniform float PowerFactor)
{
    float4 grassColor = tex2D(GrassSampler, vertexGroundTCoord);
    float4 stoneColor = tex2D(StoneSampler, vertexGroundTCoord);
    float4 blendColor = tex1D(BlendSampler, vertexBlendTCoord);

    float2 offsetCloudTCoord = vertexCloudTCoord + vertexFlowDirection;    
    float4 cloudColor = tex2D(CloudSampler, offsetCloudTCoord);

    float stoneWeight = pow(blendColor.r, PowerFactor);
    float grassWeight = 1.0f - stoneWeight;
    pixelColor = cloudColor*(grassWeight*grassColor + stoneWeight*stoneColor);
}
