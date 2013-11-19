// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Smoke2D.h"

//----------------------------------------------------------------------------
void Smoke2D::CreateBoundaryMixedEffect (VisualEffect*& effect,
    VisualEffectInstance*& instance)
{
    PixelShader* pshader = new0 PixelShader("Wm5.BoundaryMixed2",
        1, 1, 0, 3, false);
    pshader->SetInput(0, "vertexTCoord", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    pshader->SetOutput(0, "pixelColor", Shader::VT_FLOAT4,
        Shader::VS_COLOR0);
    pshader->SetSampler(0, "MaskSampler", Shader::ST_2D);
    pshader->SetSampler(1, "OffsetSampler", Shader::ST_2D);
    pshader->SetSampler(2, "StateSampler", Shader::ST_2D);
    pshader->SetTextureUnits(msBoundaryMixedPTextureUnits);
    pshader->SetPrograms(msBoundaryMixedPPrograms);

    mIP->CreateEffect(pshader, effect, instance);

    // Create the mask and offset textures.
    Texture2D* maskMixed = new0 Texture2D(Texture::TF_A32B32G32R32F,
        mIMaxP1, mJMaxP1, 1);
    Texture2D* offsetMixed = new0 Texture2D(Texture::TF_A32B32G32R32F,
        mIMaxP1, mJMaxP1, 1);

    Float4* mask = (Float4*)maskMixed->GetData(0);
    Float4* offset = (Float4*)offsetMixed->GetData(0);
    Float4 one(1.0f, 1.0f, 1.0f, 1.0f);
    Float4 zero(0.0f, 0.0f, 0.0f, 0.0f);
    Float4 xEdgeMask(0.0f, 1.0f, 0.0f, 0.0f);
    Float4 x0EdgeOffset(+mDx, 0.0f, 0.0f, 0.0f);
    Float4 x1EdgeOffset(-mDx, 0.0f, 0.0f, 0.0f);
    Float4 yEdgeMask(1.0f, 0.0f, 0.0f, 0.0f);
    Float4 y0EdgeOffset(0.0f, +mDy, 0.0f, 0.0f);
    Float4 y1EdgeOffset(0.0f, -mDy, 0.0f, 0.0f);
    int x, y;

    // Interior.
    for (y = 1; y < mJMax; ++y)
    {
        for (x = 1; x < mIMax; ++x)
        {
            mask[Index(x,y)] = one;
            offset[Index(x,y)] = zero;
        }
    }

    // Edge-interior.
    for (x = 1; x < mIMax; ++x)
    {
        mask[Index(x,0)] = yEdgeMask;
        offset[Index(x,0)] = y0EdgeOffset;
        mask[Index(x,mJMax)] = yEdgeMask;
        offset[Index(x,mJMax)] = y1EdgeOffset;
    }
    for (y = 1; y < mJMax; ++y)
    {
        mask[Index(0,y)] = xEdgeMask;
        offset[Index(0,y)] = x0EdgeOffset;
        mask[Index(mIMax,y)] = xEdgeMask;
        offset[Index(mIMax,y)] = x1EdgeOffset;
    }

    // Corners.
    mask[Index(0,0)] = zero;
    mask[Index(mIMax,0)] = zero;
    mask[Index(0,mJMax)] = zero;
    mask[Index(mIMax,mJMax)] = zero;
    offset[Index(0,0)] = zero;
    offset[Index(mIMax,0)] = zero;
    offset[Index(0,mJMax)] = zero;
    offset[Index(mIMax,mJMax)] = zero;

    instance->SetPixelTexture(0, "MaskSampler", maskMixed);
    instance->SetPixelTexture(0, "OffsetSampler", offsetMixed);
    instance->SetPixelTexture(0, "StateSampler",
        mIP->GetTarget(2)->GetColorTexture(0));

    mRenderer->Bind(maskMixed);
    mRenderer->Bind(offsetMixed);
}
//----------------------------------------------------------------------------
void Smoke2D::CreateCopyStateEffect (VisualEffect*& effect,
    VisualEffectInstance*& instance)
{
    PixelShader* pshader = new0 PixelShader("Wm5.CopyState2",
        1, 1, 0, 1, false);
    pshader->SetInput(0, "vertexTCoord", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    pshader->SetOutput(0, "pixelColor", Shader::VT_FLOAT4,
        Shader::VS_COLOR0);
    pshader->SetSampler(0, "StateSampler", Shader::ST_2D);
    pshader->SetTextureUnits(msCopyStatePTextureUnits);
    pshader->SetPrograms(msCopyStatePPrograms);

    mIP->CreateEffect(pshader, effect, instance);

    mInitialTexture = new0 Texture2D(Texture::TF_A32B32G32R32F, mIMaxP1,
        mJMaxP1, 1);

    if (PixelShader::GetProfile() == PixelShader::PP_ARBFP1)
    {
        float* data = (float*)mInitialTexture->GetData(0);
        for (int j = 0; j <= mJMax; ++j)
        {
            for (int i = 0; i <= mIMax; ++i)
            {
                *data++ = 0.0f;  // velocity.x
                *data++ = 0.0f;  // velocity.y
                *data++ = 0.0f;  // empty
                *data++ = Mathf::UnitRandom();
            }
        }
    }
    else
    {
        Float4* data = (Float4*)mInitialTexture->GetData(0);
        for (int j = 0; j <= mJMax; ++j)
        {
            for (int i = 0; i <= mIMax; ++i)
            {
                data[i + mIMaxP1*(mJMax-j)] = Float4(0.0f, 0.0f, 0.0f,
                    Mathf::UnitRandom());
            }
        }
    }

    mRenderer->Bind(mInitialTexture);
}
//----------------------------------------------------------------------------
void Smoke2D::CreateFluidUpdateEffect (VisualEffect*& effect,
    VisualEffectInstance*& instance)
{
    PixelShader* pshader = new0 PixelShader("Wm5.FluidUpdate2",
        1, 1, 3, 3, false);
    pshader->SetInput(0, "vertexTCoord", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    pshader->SetOutput(0, "pixelColor", Shader::VT_FLOAT4,
        Shader::VS_COLOR0);
    pshader->SetConstant(0, "SpaceParam", 1);
    pshader->SetConstant(1, "TimeParam", 1);
    pshader->SetConstant(2, "ViscParam", 1);
    pshader->SetSampler(0, "StateSampler", Shader::ST_2D);
    pshader->SetSampler(1, "AdvectionSampler", Shader::ST_2D);
    pshader->SetSampler(2, "SourceSampler", Shader::ST_2D);
    pshader->SetFilter(1, Shader::SF_LINEAR);
    pshader->SetBaseRegisters(msFluidUpdatePRegisters);
    pshader->SetTextureUnits(msFluidUpdatePTextureUnits);
    pshader->SetPrograms(msFluidUpdatePPrograms);

    mIP->CreateEffect(pshader, effect, instance);

    ShaderFloat* spaceParamConstant = new0 ShaderFloat(1);
    float* spaceParam = spaceParamConstant->GetData();
    spaceParam[0] = mDx;
    spaceParam[1] = mDy;
    spaceParam[2] = 1.0f/(float)mIMax;
    spaceParam[3] = 1.0f/(float)mJMax;
    instance->SetPixelConstant(0, "SpaceParam", spaceParamConstant);

    ShaderFloat* timeParamConstant = new0 ShaderFloat(1);
    float* timeParam = timeParamConstant->GetData();
    timeParam[0] = mDtDivDx;
    timeParam[1] = mDtDivDy;
    timeParam[2] = mDt;
    instance->SetPixelConstant(0, "TimeParam", timeParamConstant);

    ShaderFloat* viscParamConstant = new0 ShaderFloat(1);
    float* viscParam = viscParamConstant->GetData();
    viscParam[0] = mDenLambdaX;
    viscParam[1] = mDenLambdaY;
    viscParam[2] = mVelLambdaX;
    viscParam[3] = mVelLambdaY;
    instance->SetPixelConstant(0, "ViscParam", viscParamConstant);

    mSourceTexture = new0 Texture2D(Texture::TF_A32B32G32R32F, mIMaxP1,
        mJMaxP1, 1);
    ComputeSource();

    instance->SetPixelTexture(0, "StateSampler",
        mIP->GetTarget(1)->GetColorTexture(0));
    instance->SetPixelTexture(0, "AdvectionSampler",
        mIP->GetTarget(4)->GetColorTexture(0));
    instance->SetPixelTexture(0, "SourceSampler", mSourceTexture);

    mRenderer->Bind(mSourceTexture);
}
//----------------------------------------------------------------------------
void Smoke2D::CreateDivergenceEffect (VisualEffect*& effect,
    VisualEffectInstance*& instance)
{
    PixelShader* pshader = new0 PixelShader("Wm5.Divergence2",
        1, 1, 1, 1, false);
    pshader->SetInput(0, "vertexTCoord", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    pshader->SetOutput(0, "pixelColor", Shader::VT_FLOAT4,
        Shader::VS_COLOR0);
    pshader->SetConstant(0, "SpaceParam", 1);
    pshader->SetSampler(0, "StateSampler", Shader::ST_2D);
    pshader->SetBaseRegisters(msDivergencePRegisters);
    pshader->SetTextureUnits(msDivergencePTextureUnits);
    pshader->SetPrograms(msDivergencePPrograms);

    mIP->CreateEffect(pshader, effect, instance);

    ShaderFloat* spaceParamConstant = new0 ShaderFloat(1);
    float* spaceParam = spaceParamConstant->GetData();
    spaceParam[0] = mDx;
    spaceParam[1] = mDy;
    spaceParam[2] = mHalfDivDx;
    spaceParam[3] = mHalfDivDy;
    instance->SetPixelConstant(0, "SpaceParam", spaceParamConstant);

    instance->SetPixelTexture(0, "StateSampler",
        mIP->GetTarget(1)->GetColorTexture(0));
}
//----------------------------------------------------------------------------
void Smoke2D::CreatePoissonSolverEffect (VisualEffect*& effect,
    VisualEffectInstance*& instance)
{
    PixelShader* pshader = new0 PixelShader("Wm5.PoissonSolver2",
        1, 1, 2, 2, false);
    pshader->SetInput(0, "vertexTCoord", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    pshader->SetOutput(0, "pixelColor", Shader::VT_FLOAT4,
        Shader::VS_COLOR0);
    pshader->SetConstant(0, "SpaceParam", 1);
    pshader->SetConstant(1, "EpsilonParam", 1);
    pshader->SetSampler(0, "DivergenceSampler", Shader::ST_2D);
    pshader->SetSampler(1, "PoissonSampler", Shader::ST_2D);
    pshader->SetBaseRegisters(msPoissonSolverPRegisters);
    pshader->SetTextureUnits(msPoissonSolverPTextureUnits);
    pshader->SetPrograms(msPoissonSolverPPrograms);

    mIP->CreateEffect(pshader, effect, instance);

    ShaderFloat* spaceParamConstant = new0 ShaderFloat(1);
    float* spaceParam = spaceParamConstant->GetData();
    spaceParam[0] = mDx;
    spaceParam[1] = mDy;
    instance->SetPixelConstant(0, "SpaceParam", spaceParamConstant);

    ShaderFloat* epsilonParamConstant = new0 ShaderFloat(1);
    float* epsilonParam = epsilonParamConstant->GetData();
    epsilonParam[0] = mEpsilonX;
    epsilonParam[1] = mEpsilonY;
    epsilonParam[2] = mEpsilon0;
    instance->SetPixelConstant(0, "EpsilonParam", epsilonParamConstant);

    instance->SetPixelTexture(0, "DivergenceSampler",
        mIP->GetTarget(2)->GetColorTexture(0));
    instance->SetPixelTexture(0, "PoissonSampler",
        mIP->GetTarget(0)->GetColorTexture(0));
}
//----------------------------------------------------------------------------
void Smoke2D::CreateAdjustVelocityEffect (VisualEffect*& effect,
    VisualEffectInstance*& instance)
{
    PixelShader* pshader = new0 PixelShader("Wm5.AdjustVelocity2",
        1, 1, 1, 2, false);
    pshader->SetInput(0, "vertexTCoord", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    pshader->SetOutput(0, "pixelColor", Shader::VT_FLOAT4,
        Shader::VS_COLOR0);
    pshader->SetConstant(0, "SpaceParam", 1);
    pshader->SetSampler(0, "StateSampler", Shader::ST_2D);
    pshader->SetSampler(1, "PoissonSampler", Shader::ST_2D);
    pshader->SetBaseRegisters(msAdjustVelocityPRegisters);
    pshader->SetTextureUnits(msAdjustVelocityPTextureUnits);
    pshader->SetPrograms(msAdjustVelocityPPrograms);

    mIP->CreateEffect(pshader, effect, instance);

    ShaderFloat* spaceParamConstant = new0 ShaderFloat(1);
    float* spaceParam = spaceParamConstant->GetData();
    spaceParam[0] = mDx;
    spaceParam[1] = mDy;
    spaceParam[2] = mHalfDivDx;
    spaceParam[3] = mHalfDivDy;
    instance->SetPixelConstant(0, "SpaceParam", spaceParamConstant);

    instance->SetPixelTexture(0, "StateSampler",
        mIP->GetTarget(1)->GetColorTexture(0));
    instance->SetPixelTexture(0, "PoissonSampler",
        mIP->GetTarget(0)->GetColorTexture(0));
}
//----------------------------------------------------------------------------
void Smoke2D::CreateDrawDensityEffect (VisualEffect*& effect,
    VisualEffectInstance*& instance)
{
    PixelShader* pshader = new0 PixelShader("Wm5.DrawDensity2",
        1, 1, 0, 3, false);
    pshader->SetInput(0, "vertexTCoord", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    pshader->SetOutput(0, "pixelColor", Shader::VT_FLOAT4,
        Shader::VS_COLOR0);
    pshader->SetSampler(0, "StateSampler", Shader::ST_2D);
    pshader->SetSampler(1, "ColorTableSampler", Shader::ST_1D);
    pshader->SetSampler(2, "VortexSampler", Shader::ST_2D);
    pshader->SetFilter(1, Shader::SF_LINEAR);
    pshader->SetTextureUnits(msDrawDensityPTextureUnits);
    pshader->SetPrograms(msDrawDensityPPrograms);

    mIP->CreateEffect(pshader, effect, instance);

    // For pseudocoloring density.
    mGrayTexture = new0 Texture1D(Texture::TF_A8R8G8B8, 8, 1);
    unsigned char* gray = (unsigned char*)mGrayTexture->GetData(0);
    gray[ 0] =   0;  gray[ 1] =   0;  gray[ 2] =   0;  gray[ 3] = 255;
    gray[ 4] =  32;  gray[ 5] =  32;  gray[ 6] =  32;  gray[ 7] = 255;
    gray[ 8] =  64;  gray[ 9] =  64;  gray[10] =  64;  gray[11] = 255;
    gray[12] =  96;  gray[13] =  96;  gray[14] =  96;  gray[15] = 255;
    gray[16] = 128;  gray[17] = 128;  gray[18] = 128;  gray[19] = 255;
    gray[20] = 160;  gray[21] = 160;  gray[22] = 160;  gray[23] = 255;
    gray[24] = 192;  gray[25] = 192;  gray[26] = 192;  gray[27] = 255;
    gray[28] = 255;  gray[29] = 255;  gray[30] = 255;  gray[31] = 255;

    mColorTexture = new0 Texture1D(Texture::TF_A8R8G8B8, 8, 1);
    unsigned char* color = (unsigned char*)mColorTexture->GetData(0);
    color[ 0] =   0;  color[ 1] =   0;  color[ 2] =   0;  color[ 3] = 255;
    color[ 4] = 255;  color[ 5] =   0;  color[ 6] = 128;  color[ 7] = 255;
    color[ 8] = 255;  color[ 9] =   0;  color[10] =   0;  color[11] = 255;
    color[12] =   0;  color[13] = 255;  color[14] =   0;  color[15] = 255;
    color[16] =   0;  color[17] = 255;  color[18] = 255;  color[19] = 255;
    color[20] =   0;  color[21] = 128;  color[22] = 255;  color[23] = 255;
    color[24] =   0;  color[25] =   0;  color[26] = 255;  color[27] = 255;
    color[28] = 255;  color[29] = 255;  color[30] = 255;  color[31] = 255;

    // For vortex-center overlay.
    mVortexTexture = new0 Texture2D(Texture::TF_A8R8G8B8, mIMaxP1,
        mJMaxP1, 1);
    memset(mVortexTexture->GetData(0), 0, 4*mNumPixels);

    instance->SetPixelTexture(0, "StateSampler",
        mIP->GetTarget(1)->GetColorTexture(0));
    instance->SetPixelTexture(0, "ColorTableSampler", mGrayTexture);
    instance->SetPixelTexture(0, "VortexSampler", mVortexTexture);

    mRenderer->Bind(mGrayTexture);
    mRenderer->Bind(mColorTexture);
    mRenderer->Bind(mVortexTexture);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Boundary mixed profiles.
//----------------------------------------------------------------------------
int Smoke2D::msAllBoundaryMixedPTextureUnits[3] = { 0, 1, 2};
int* Smoke2D::msBoundaryMixedPTextureUnits[Shader::MAX_PROFILES] =
{
    0,
    msAllBoundaryMixedPTextureUnits,
    msAllBoundaryMixedPTextureUnits,
    msAllBoundaryMixedPTextureUnits,
    msAllBoundaryMixedPTextureUnits
};

std::string Smoke2D::msBoundaryMixedPPrograms[Shader::MAX_PROFILES] =
{
    // PP_NONE
    "",

    // PP_PS_1_1
    "",

    // PP_PS_2_0
    "ps_2_0\n"
    "dcl_2d s0\n"
    "dcl_2d s1\n"
    "dcl_2d s2\n"
    "dcl t0.xy\n"
    "texld r0, t0, s1\n"
    "texld r1, t0, s0\n"
    "add r0.xy, t0, r0\n"
    "texld r0, r0, s2\n"
    "mul r0, r1, r0\n"
    "mov oC0, r0\n",

    // PP_PS_3_0
    "ps_3_0\n"
    "dcl_2d s0\n"
    "dcl_2d s1\n"
    "dcl_2d s2\n"
    "dcl_texcoord0 v0.xy\n"
    "texld r0.xy, v0, s1\n"
    "add r0.xy, v0, r0\n"
    "texld r1, r0, s2\n"
    "texld r0, v0, s0\n"
    "mul oC0, r0, r1\n",

    // PP_ARBFP1
    "!!ARBfp1.0\n"
    "TEMP R0;\n"
    "TEMP R1;\n"
    "TEX R0.xy, fragment.texcoord[0], texture[1], 2D;\n"
    "ADD R0.xy, fragment.texcoord[0], R0;\n"
    "TEX R1, R0, texture[2], 2D;\n"
    "TEX R0, fragment.texcoord[0], texture[0], 2D;\n"
    "MUL result.color, R0, R1;\n"
    "END\n"
};
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Copy state profiles.
//----------------------------------------------------------------------------
int Smoke2D::msAllCopyStatePTextureUnits[1] = { 0 };
int* Smoke2D::msCopyStatePTextureUnits[Shader::MAX_PROFILES] =
{
    0,
    msAllCopyStatePTextureUnits,
    msAllCopyStatePTextureUnits,
    msAllCopyStatePTextureUnits,
    msAllCopyStatePTextureUnits
};

std::string Smoke2D::msCopyStatePPrograms[Shader::MAX_PROFILES] =
{
    // PP_NONE
    "",

    // PP_PS_1_1
    "",

    // PP_PS_2_0
    "ps_2_0\n"
    "dcl_2d s0\n"
    "dcl t0.xy\n"
    "texld r0, t0, s0\n"
    "mov oC0, r0\n",

    // PP_PS_3_0
    "ps_3_0\n"
    "dcl_2d s0\n"
    "dcl_texcoord0 v0.xy\n"
    "texld r0, v0, s0\n"
    "mov oC0, r0\n",

    // PP_ARBFP1
    "!!ARBfp1.0\n"
    "TEX result.color, fragment.texcoord[0], texture[0], 2D;\n"
    "END\n"
};
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Fluid update profiles.
//----------------------------------------------------------------------------
int Smoke2D::msAllFluidUpdatePRegisters[3] = { 0, 1, 2 };
int* Smoke2D::msFluidUpdatePRegisters[Shader::MAX_PROFILES] =
{
    0,
    msAllFluidUpdatePRegisters,
    msAllFluidUpdatePRegisters,
    msAllFluidUpdatePRegisters,
    msAllFluidUpdatePRegisters
};

int Smoke2D::msAllFluidUpdatePTextureUnits[3] = { 0, 1, 2 };
int* Smoke2D::msFluidUpdatePTextureUnits[Shader::MAX_PROFILES] =
{
    0,
    msAllFluidUpdatePTextureUnits,
    msAllFluidUpdatePTextureUnits,
    msAllFluidUpdatePTextureUnits,
    msAllFluidUpdatePTextureUnits
};

std::string Smoke2D::msFluidUpdatePPrograms[Shader::MAX_PROFILES] =
{
    // PP_NONE
    "",

    // PP_PS_1_1
    "",

    // PP_PS_2_0
    "ps_2_0\n"
    "dcl_2d s0\n"
    "dcl_2d s1\n"
    "dcl_2d s2\n"
    "def c3, 2.00000000, 0.00000000, 0, 0\n"
    "dcl t0.xy\n"
    "texld r6, t0, s0\n"
    "add r2.xy, t0, -c0\n"
    "mov r0.y, c1\n"
    "mov r4.x, r2\n"
    "mov r0.x, c1\n"
    "mov r2.x, t0\n"
    "mov r4.y, t0\n"
    "mul r1.x, c0.w, r0.y\n"
    "mad r0.y, r6, -r1.x, t0\n"
    "add r1.xy, t0, c0\n"
    "mul r0.x, c0.z, r0\n"
    "mad r0.x, r6, -r0, t0\n"
    "mov r3.y, r1\n"
    "mov r5.x, r1\n"
    "mov r3.x, t0\n"
    "mov r5.y, t0\n"
    "texld r1, r0, s1\n"
    "texld r0, t0, s2\n"
    "texld r4, r4, s0\n"
    "texld r5, r5, s0\n"
    "texld r3, r3, s0\n"
    "texld r2, r2, s0\n"
    "mad r3.xy, -r6, c3.x, r3\n"
    "add r2.xy, r3, r2\n"
    "mul r2.xy, r2, c2.w\n"
    "mad r3.xy, -r6, c3.x, r5\n"
    "add r3.xy, r3, r4\n"
    "mad r3.xy, r3, c2.z, r2\n"
    "mad r3.xy, r0, c1.z, r3\n"
    "mad r2.x, -r6.w, c3, r3.w\n"
    "add r0.x, r2.w, r2\n"
    "mad r2.x, -r6.w, c3, r5.w\n"
    "mul r0.x, r0, c2.y\n"
    "add r2.x, r4.w, r2\n"
    "mad r0.x, r2, c2, r0\n"
    "mad r0.x, r0.w, c1.z, r0\n"
    "mad r1.xy, r3, c3.x, r1\n"
    "mad r1.w, r0.x, c3.x, r1\n"
    "mov r1.z, c3.y\n"
    "mov oC0, r1\n",

    // PP_PS_3_0
    "ps_3_0\n"
    "dcl_2d s0\n"
    "dcl_2d s1\n"
    "dcl_2d s2\n"
    "def c3, 2.00000000, 0.00000000, 0, 0\n"
    "dcl_texcoord0 v0.xy\n"
    "add r0.zw, v0.xyxy, -c0.xyxy\n"
    "add r1.xy, v0, c0\n"
    "mov r0.y, r0.w\n"
    "mov r0.x, v0\n"
    "texld r4.xyw, r0, s0\n"
    "mov r0.y, r1\n"
    "mov r0.x, v0\n"
    "texld r3.xyw, r0, s0\n"
    "texld r0.xyw, v0, s0\n"
    "mad r1.zw, -r0.xyxy, c3.x, r3.xyxy\n"
    "add r1.zw, r1, r4.xyxy\n"
    "mov r2.x, r0.z\n"
    "mov r2.y, v0\n"
    "mul r3.xy, r1.zwzw, c2.w\n"
    "mad r0.z, -r0.w, c3.x, r3.w\n"
    "mov r1.y, v0\n"
    "texld r1.xyw, r1, s0\n"
    "add r1.z, r4.w, r0\n"
    "mad r0.z, -r0.w, c3.x, r1.w\n"
    "texld r2.xyw, r2, s0\n"
    "mad r1.xy, -r0, c3.x, r1\n"
    "add r1.xy, r1, r2\n"
    "mad r1.xy, r1, c2.z, r3\n"
    "texld r3.xyw, v0, s2\n"
    "mul r0.w, r1.z, c2.y\n"
    "add r0.z, r2.w, r0\n"
    "mad r1.z, r0, c2.x, r0.w\n"
    "mov r0.w, c1.y\n"
    "mul r0.w, c0, r0\n"
    "mov r0.z, c1.x\n"
    "mul r0.z, c0, r0\n"
    "mad r0.x, r0, -r0.z, v0\n"
    "mad r0.y, r0, -r0.w, v0\n"
    "texld r0.xyw, r0, s1\n"
    "mad r1.xy, r3, c1.z, r1\n"
    "mad r0.z, r3.w, c1, r1\n"
    "mad oC0.xy, r1, c3.x, r0\n"
    "mad oC0.w, r0.z, c3.x, r0\n"
    "mov oC0.z, c3.y\n",

    // PP_ARBFP1
    "!!ARBfp1.0\n"
    "PARAM c[4] = { program.local[0..2],\n"
    "		{ 0, 2 } };\n"
    "TEMP R0;\n"
    "TEMP R1;\n"
    "TEMP R2;\n"
    "TEMP R3;\n"
    "TEMP R4;\n"
    "TEMP R5;\n"
    "ADD R0.zw, fragment.texcoord[0].xyxy, -c[0].xyxy;\n"
    "ADD R1.xy, fragment.texcoord[0], c[0];\n"
    "MOV R0.y, R0.w;\n"
    "MOV R0.x, fragment.texcoord[0];\n"
    "TEX R4.xyw, R0, texture[0], 2D;\n"
    "MOV R0.y, R1;\n"
    "MOV R0.x, fragment.texcoord[0];\n"
    "TEX R3.xyw, R0, texture[0], 2D;\n"
    "TEX R0.xyw, fragment.texcoord[0], texture[0], 2D;\n"
    "MAD R1.zw, -R0.xyxy, c[3].y, R3.xyxy;\n"
    "ADD R1.zw, R1, R4.xyxy;\n"
    "MUL R3.xy, R1.zwzw, c[2].w;\n"
    "MOV R1.y, fragment.texcoord[0];\n"
    "TEX R1.xyw, R1, texture[0], 2D;\n"
    "MOV R2.x, R0.z;\n"
    "MOV R2.y, fragment.texcoord[0];\n"
    "MAD R0.z, -R0.w, c[3].y, R1.w;\n"
    "MAD R1.z, -R0.w, c[3].y, R3.w;\n"
    "ADD R0.w, R4, R1.z;\n"
    "TEX R2.xyw, R2, texture[0], 2D;\n"
    "MAD R1.xy, -R0, c[3].y, R1;\n"
    "ADD R1.xy, R1, R2;\n"
    "TEX R5.xyw, fragment.texcoord[0], texture[2], 2D;\n"
    "MAD R1.xy, R1, c[2].z, R3;\n"
    "ADD R1.z, R2.w, R0;\n"
    "MUL R1.w, R0, c[2].y;\n"
    "MOV R0.zw, c[0];\n"
    "MUL R0.w, R0, c[1].y;\n"
    "MUL R0.z, R0, c[1].x;\n"
    "MAD R0.x, R0, -R0.z, fragment.texcoord[0];\n"
    "MAD R1.z, R1, c[2].x, R1.w;\n"
    "MAD R0.y, R0, -R0.w, fragment.texcoord[0];\n"
    "TEX R0.xyw, R0, texture[1], 2D;\n"
    "MAD R1.xy, R5, c[1].z, R1;\n"
    "MAD R0.z, R5.w, c[1], R1;\n"
    "MAD result.color.xy, R1, c[3].y, R0;\n"
    "MAD result.color.w, R0.z, c[3].y, R0;\n"
    "MOV result.color.z, c[3].x;\n"
    "END\n"
};
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Divergence profiles.
//----------------------------------------------------------------------------
int Smoke2D::msAllDivergencePRegisters[1] = { 0 };
int* Smoke2D::msDivergencePRegisters[Shader::MAX_PROFILES] =
{
    0,
    msAllDivergencePRegisters,
    msAllDivergencePRegisters,
    msAllDivergencePRegisters,
    msAllDivergencePRegisters
};

int Smoke2D::msAllDivergencePTextureUnits[1] = { 0 };
int* Smoke2D::msDivergencePTextureUnits[Shader::MAX_PROFILES] =
{
    0,
    msAllDivergencePTextureUnits,
    msAllDivergencePTextureUnits,
    msAllDivergencePTextureUnits,
    msAllDivergencePTextureUnits
};

std::string Smoke2D::msDivergencePPrograms[Shader::MAX_PROFILES] =
{
    // PP_NONE
    "",

    // PP_PS_1_1
    "",

    // PP_PS_2_0
    "ps_2_0\n"
    "dcl_2d s0\n"
    "dcl t0.xy\n"
    "add r1.xy, t0, c0\n"
    "mov r2.x, r1\n"
    "mov r2.y, t0\n"
    "add r0.xy, t0, -c0\n"
    "mov r3.y, r1\n"
    "mov r1.y, r0\n"
    "mov r3.x, t0\n"
    "mov r1.x, t0\n"
    "mov r0.y, t0\n"
    "texld r0, r0, s0\n"
    "texld r1, r1, s0\n"
    "texld r2, r2, s0\n"
    "texld r3, r3, s0\n"
    "add r1.x, r3.y, -r1.y\n"
    "mul r1.x, r1, c0.w\n"
    "add r0.x, r2, -r0\n"
    "mad r0.x, r0, c0.z, r1\n"
    "mov r0, r0.x\n"
    "mov oC0, r0\n",

    // PP_PS_3_0
    "ps_3_0\n"
    "dcl_2d s0\n"
    "dcl_texcoord0 v0.xy\n"
    "add r0.xy, v0, -c0\n"
    "mov r1.y, r0\n"
    "mov r1.x, v0\n"
    "texld r0.y, r1, s0\n"
    "add r0.zw, v0.xyxy, c0.xyxy\n"
    "mov r1.x, v0\n"
    "mov r1.y, r0.w\n"
    "texld r1.y, r1, s0\n"
    "add r0.y, r1, -r0\n"
    "mov r1.x, r0\n"
    "mov r1.y, v0\n"
    "texld r0.x, r1, s0\n"
    "mul r0.y, r0, c0.w\n"
    "mov r1.y, v0\n"
    "mov r1.x, r0.z\n"
    "texld r1.x, r1, s0\n"
    "add r0.x, r1, -r0\n"
    "mad oC0, r0.x, c0.z, r0.y\n",

    // PP_ARBFP1
    "!!ARBfp1.0\n"
    "PARAM c[1] = { program.local[0] };\n"
    "TEMP R0;\n"
    "TEMP R1;\n"
    "ADD R0.xy, fragment.texcoord[0], -c[0];\n"
    "ADD R0.zw, fragment.texcoord[0].xyxy, c[0].xyxy;\n"
    "MOV R1.y, R0;\n"
    "MOV R1.x, fragment.texcoord[0];\n"
    "TEX R0.y, R1, texture[0], 2D;\n"
    "MOV R1.y, R0.w;\n"
    "MOV R1.x, fragment.texcoord[0];\n"
    "TEX R1.y, R1, texture[0], 2D;\n"
    "ADD R0.y, R1, -R0;\n"
    "MOV R1.x, R0;\n"
    "MOV R1.y, fragment.texcoord[0];\n"
    "TEX R0.x, R1, texture[0], 2D;\n"
    "MOV R0.w, fragment.texcoord[0].y;\n"
    "TEX R1.x, R0.zwzw, texture[0], 2D;\n"
    "MUL R0.y, R0, c[0].w;\n"
    "ADD R0.x, R1, -R0;\n"
    "MAD result.color, R0.x, c[0].z, R0.y;\n"
    "END\n"
};
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Poisson solver profiles.
//----------------------------------------------------------------------------
int Smoke2D::msAllPoissonSolverPRegisters[2] = { 0, 1 };
int* Smoke2D::msPoissonSolverPRegisters[Shader::MAX_PROFILES] =
{
    0,
    msAllPoissonSolverPRegisters,
    msAllPoissonSolverPRegisters,
    msAllPoissonSolverPRegisters,
    msAllPoissonSolverPRegisters
};

int Smoke2D::msAllPoissonSolverPTextureUnits[2] = { 0, 1 };
int* Smoke2D::msPoissonSolverPTextureUnits[Shader::MAX_PROFILES] =
{
    0,
    msAllPoissonSolverPTextureUnits,
    msAllPoissonSolverPTextureUnits,
    msAllPoissonSolverPTextureUnits,
    msAllPoissonSolverPTextureUnits
};

std::string Smoke2D::msPoissonSolverPPrograms[Shader::MAX_PROFILES] =
{
    // PP_NONE
    "",

    // PP_PS_1_1
    "",

    // PP_PS_2_0
    "ps_2_0\n"
    "dcl_2d s0\n"
    "dcl_2d s1\n"
    "dcl t0.xy\n"
    "texld r4, t0, s0\n"
    "add r1.xy, t0, c0\n"
    "mov r2.y, r1\n"
    "mov r2.x, t0\n"
    "add r0.xy, t0, -c0\n"
    "mov r3.x, r1\n"
    "mov r1.x, r0\n"
    "mov r3.y, t0\n"
    "mov r1.y, t0\n"
    "mov r0.x, t0\n"
    "texld r0, r0, s1\n"
    "texld r2, r2, s1\n"
    "texld r1, r1, s1\n"
    "texld r3, r3, s1\n"
    "add r1.x, r3, r1\n"
    "mul r1.x, r1, c1\n"
    "mad r1.x, r4, c1.z, r1\n"
    "add r0.x, r2, r0\n"
    "mad r0.x, r0, c1.y, r1\n"
    "mov r0, r0.x\n"
    "mov oC0, r0\n",

    // PP_PS_3_0
    "ps_3_0\n"
    "dcl_2d s0\n"
    "dcl_2d s1\n"
    "dcl_texcoord0 v0.xy\n"
    "add r0.xy, v0, -c0\n"
    "mov r1.x, r0\n"
    "mov r1.y, v0\n"
    "texld r0.x, r1, s1\n"
    "add r0.zw, v0.xyxy, c0.xyxy\n"
    "mov r1.y, v0\n"
    "mov r1.x, r0.z\n"
    "texld r1.x, r1, s1\n"
    "add r0.x, r1, r0\n"
    "mul r0.z, r0.x, c1.x\n"
    "texld r0.x, v0, s0\n"
    "mad r0.z, r0.x, c1, r0\n"
    "mov r0.x, v0\n"
    "texld r0.x, r0, s1\n"
    "mov r1.y, r0.w\n"
    "mov r1.x, v0\n"
    "texld r1.x, r1, s1\n"
    "add r0.x, r1, r0\n"
    "mad oC0, r0.x, c1.y, r0.z\n",

    // PP_ARBFP1
    "!!ARBfp1.0\n"
    "PARAM c[2] = { program.local[0..1] };\n"
    "TEMP R0;\n"
    "TEMP R1;\n"
    "ADD R1.xy, fragment.texcoord[0], -c[0];\n"
    "ADD R0.zw, fragment.texcoord[0].xyxy, c[0].xyxy;\n"
    "MOV R1.z, R0;\n"
    "MOV R0.y, fragment.texcoord[0];\n"
    "MOV R0.x, R1;\n"
    "MOV R1.w, fragment.texcoord[0].y;\n"
    "TEX R0.x, R0, texture[1], 2D;\n"
    "TEX R1.x, R1.zwzw, texture[1], 2D;\n"
    "ADD R0.x, R1, R0;\n"
    "MUL R0.y, R0.x, c[1].x;\n"
    "TEX R0.x, fragment.texcoord[0], texture[0], 2D;\n"
    "MAD R0.y, R0.x, c[1].z, R0;\n"
    "MOV R1.x, fragment.texcoord[0];\n"
    "TEX R0.x, R1, texture[1], 2D;\n"
    "MOV R0.z, fragment.texcoord[0].x;\n"
    "TEX R1.x, R0.zwzw, texture[1], 2D;\n"
    "ADD R0.x, R1, R0;\n"
    "MAD result.color, R0.x, c[1].y, R0.y;\n"
    "END\n"
};
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Adjust velocity profiles.
//----------------------------------------------------------------------------
int Smoke2D::msAllAdjustVelocityPRegisters[1] = { 0 };
int* Smoke2D::msAdjustVelocityPRegisters[Shader::MAX_PROFILES] =
{
    0,
    msAllAdjustVelocityPRegisters,
    msAllAdjustVelocityPRegisters,
    msAllAdjustVelocityPRegisters,
    msAllAdjustVelocityPRegisters
};

int Smoke2D::msAllAdjustVelocityPTextureUnits[2] = { 0, 1 };
int* Smoke2D::msAdjustVelocityPTextureUnits[Shader::MAX_PROFILES] =
{
    0,
    msAllAdjustVelocityPTextureUnits,
    msAllAdjustVelocityPTextureUnits,
    msAllAdjustVelocityPTextureUnits,
    msAllAdjustVelocityPTextureUnits
};

std::string Smoke2D::msAdjustVelocityPPrograms[Shader::MAX_PROFILES] =
{
    // PP_NONE
    "",

    // PP_PS_1_1
    "",

    // PP_PS_2_0
    "ps_2_0\n"
    "dcl_2d s0\n"
    "dcl_2d s1\n"
    "def c1, 0.00000000, 0, 0, 0\n"
    "dcl t0.xy\n"
    "add r0.xy, t0, c0\n"
    "mov r3.y, r0\n"
    "mov r3.x, t0\n"
    "add r1.xy, t0, -c0\n"
    "mov r2.x, r0\n"
    "mov r0.x, r1\n"
    "mov r2.y, t0\n"
    "mov r0.y, t0\n"
    "mov r1.x, t0\n"
    "texld r4, r3, s1\n"
    "texld r3, r1, s1\n"
    "texld r0, r0, s1\n"
    "texld r1, t0, s0\n"
    "texld r2, r2, s1\n"
    "add r0.x, r2, -r0\n"
    "add r2.x, r4.y, -r3.y\n"
    "mad r0.y, r2.x, c0.w, r1\n"
    "mad r0.x, r0, c0.z, r1\n"
    "mov r0.w, r1\n"
    "mov r0.z, c1.x\n"
    "mov oC0, r0\n",

    // PP_PS_3_0
    "ps_3_0\n"
    "dcl_2d s0\n"
    "dcl_2d s1\n"
    "def c1, 0.00000000, 0, 0, 0\n"
    "dcl_texcoord0 v0.xy\n"
    "add r0.xy, v0, -c0\n"
    "texld r2.xyw, v0, s0\n"
    "mov r1.x, r0\n"
    "mov r1.y, v0\n"
    "texld r0.x, r1, s1\n"
    "add r0.zw, v0.xyxy, c0.xyxy\n"
    "mov r1.y, v0\n"
    "mov r1.x, r0.z\n"
    "texld r1.x, r1, s1\n"
    "add r0.z, r1.x, -r0.x\n"
    "mov r0.x, v0\n"
    "texld r0.y, r0, s1\n"
    "mov r1.y, r0.w\n"
    "mov r1.x, v0\n"
    "texld r1.y, r1, s1\n"
    "add r0.x, r1.y, -r0.y\n"
    "mad oC0.y, r0.x, c0.w, r2\n"
    "mad oC0.x, r0.z, c0.z, r2\n"
    "mov oC0.w, r2\n"
    "mov oC0.z, c1.x\n",

    // PP_ARBFP1
    "!!ARBfp1.0\n"
    "PARAM c[2] = { program.local[0],\n"
    "		{ 0 } };\n"
    "TEMP R0;\n"
    "TEMP R1;\n"
    "TEMP R2;\n"
    "ADD R0.xy, fragment.texcoord[0], -c[0];\n"
    "TEX R2.xyw, fragment.texcoord[0], texture[0], 2D;\n"
    "MOV R0.z, R0.x;\n"
    "MOV R0.w, fragment.texcoord[0].y;\n"
    "TEX R0.x, R0.zwzw, texture[1], 2D;\n"
    "ADD R0.zw, fragment.texcoord[0].xyxy, c[0].xyxy;\n"
    "MOV R1.x, R0.z;\n"
    "MOV R1.y, fragment.texcoord[0];\n"
    "TEX R1.x, R1, texture[1], 2D;\n"
    "ADD R1.x, R1, -R0;\n"
    "MOV R0.x, fragment.texcoord[0];\n"
    "MOV R0.z, fragment.texcoord[0].x;\n"
    "TEX R0.y, R0, texture[1], 2D;\n"
    "TEX R1.y, R0.zwzw, texture[1], 2D;\n"
    "ADD R0.x, R1.y, -R0.y;\n"
    "MAD result.color.y, R0.x, c[0].w, R2;\n"
    "MAD result.color.x, R1, c[0].z, R2;\n"
    "MOV result.color.w, R2;\n"
    "MOV result.color.z, c[1].x;\n"
    "END\n"
};
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Draw density profiles.
//----------------------------------------------------------------------------
int Smoke2D::msAllDrawDensityPTextureUnits[3] = { 0, 1, 2 };
int* Smoke2D::msDrawDensityPTextureUnits[Shader::MAX_PROFILES] =
{
    0,
    msAllDrawDensityPTextureUnits,
    msAllDrawDensityPTextureUnits,
    msAllDrawDensityPTextureUnits,
    msAllDrawDensityPTextureUnits
};

std::string Smoke2D::msDrawDensityPPrograms[Shader::MAX_PROFILES] =
{
    // PP_NONE
    "",

    // PP_PS_1_1
    "",

    // PP_PS_2_0
    "ps_2_0\n"
    "dcl_2d s0\n"
    "dcl_2d s1\n"
    "dcl_2d s2\n"
    "def c0, 0.50000000, 1.00000000, 0, 0\n"
    "dcl t0.xy\n"
    "texld r0, t0, s0\n"
    "texld r1, t0, s2\n"
    "mov r0.x, r0.w\n"
    "mov r0.y, c0.x\n"
    "add r2.x, -r1.w, c0.y\n"
    "mul r1.xyz, r1.w, r1\n"
    "texld r0, r0, s1\n"
    "mad r0.xyz, r2.x, r0, r1\n"
    "mov r0.w, c0.y\n"
    "mov oC0, r0\n",

    // PP_PS_3_0
    "ps_3_0\n"
    "dcl_2d s0\n"
    "dcl_2d s1\n"
    "dcl_2d s2\n"
    "def c0, 0.50000000, 1.00000000, 0, 0\n"
    "dcl_texcoord0 v0.xy\n"
    "texld r1, v0, s2\n"
    "add r0.w, -r1, c0.y\n"
    "mul r1.xyz, r1.w, r1\n"
    "texld r1.w, v0, s0\n"
    "mov r0.y, c0.x\n"
    "mov r0.x, r1.w\n"
    "texld r0.xyz, r0, s1\n"
    "mad oC0.xyz, r0.w, r0, r1\n"
    "mov oC0.w, c0.y\n",

    // PP_ARBFP1
    "!!ARBfp1.0\n"
    "PARAM c[1] = { { 1 } };\n"
    "TEMP R0;\n"
    "TEMP R1;\n"
    "TEMP R2;\n"
    "TEX R1, fragment.texcoord[0], texture[2], 2D;\n"
    "TEX R2.w, fragment.texcoord[0], texture[0], 2D;\n"
    "ADD R0.w, -R1, c[0].x;\n"
    "MUL R1.xyz, R1.w, R1;\n"
    "TEX R0.xyz, R2.w, texture[1], 1D;\n"
    "MAD result.color.xyz, R0.w, R0, R1;\n"
    "MOV result.color.w, c[0].x;\n"
    "END\n"
};
//----------------------------------------------------------------------------
