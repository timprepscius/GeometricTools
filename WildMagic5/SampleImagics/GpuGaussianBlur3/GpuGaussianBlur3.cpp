// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "GpuGaussianBlur3.h"

WM5_WINDOW_APPLICATION(GpuGaussianBlur3);

//----------------------------------------------------------------------------
GpuGaussianBlur3::GpuGaussianBlur3 ()
    :
    WindowApplication3("SampleImagics/GpuGaussianBlur3", 0, 0, 1024, 1024,
        Float4(1.0f, 0.0f, 0.0f, 0.0f)),
        mTextColor(1.0f, 1.0f, 0.0f, 1.0f)
{
    Environment::InsertDirectory(ThePath + "Data/");

    mIP = 0;
}
//----------------------------------------------------------------------------
bool GpuGaussianBlur3::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Get any command-line parameters.
    bool useDirichlet = false;
    if (TheCommand)
    {
        // usage for Dirichlet:  GpuGaussianBlur3 -d
        // usage for Neumann:    GpuGaussianBlur3
        useDirichlet = TheCommand->GetBoolean("d") != 0;
    }

    // Load the image to be blurred.  The image is known to be 128x128x64.
#ifdef WM5_LITTLE_ENDIAN
    std::string imageName = Environment::GetPathR(
        "Head.x128y128z64.short.le.raw");
#else
    std::string imageName = Environment::GetPathR(
        "Head.x128y128z64.short.be.raw");
#endif
    assertion(imageName != "", "Cannot find or open image file\n");
    FILE* inFile = fopen(imageName.c_str(), "rb");
    int numVoxels = 128*128*64;
    short* rawData = new1<short>(numVoxels);
    fread(rawData, sizeof(short), numVoxels, inFile);
    fclose(inFile);

    // For the test image, smin = -1117 and smax = 2248.  Let us compute them
    // anyway (you would do this generally for any input image).
    short smin = rawData[0], smax = rawData[0];
    int i;
    for (int i = 1; i < numVoxels; ++i)
    {
        if (rawData[i] < smin)
        {
            smin = rawData[i];
        }
        else if (rawData[i] > smax)
        {
            smax = rawData[i];
        }
    }
    float trn = (float)smin;
    float mult = 1.0f/((float)smax - (float)smin);

    Float4* imageData = new1<Float4>(numVoxels);
    for (i = 0; i < numVoxels; ++i)
    {
        float gray = ((float)rawData[i] - trn)*mult;
        imageData[i] = Float4(gray, gray, gray, gray);
    }
    delete1(rawData);

    // Create the image processor.
    mIP = new0 ImageProcessing3(128, 128, 8, 8, imageData,
        CreateBlurPixelShader(), Float4(0.0f, 1.0f, 0.0f, 0.0f),
        useDirichlet);

    delete1(imageData);
    SetBlurInput();
#ifdef WM5_USE_OPENGL
    mIP->Initialize(mRenderer, true);
#else
    mIP->Initialize(mRenderer);
#endif
    return true;
}
//----------------------------------------------------------------------------
void GpuGaussianBlur3::OnTerminate ()
{
    delete0(mIP);
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void GpuGaussianBlur3::OnIdle ()
{
    MeasureTime();

    if (mRenderer->PreDraw())
    {
        mIP->ExecuteStep(mRenderer, true);
        DrawFrameRate(8, GetHeight()-8, mTextColor);
        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
PixelShader* GpuGaussianBlur3::CreateBlurPixelShader ()
{
    PixelShader* pshader = new0 PixelShader("Wm5.GaussianBlur3",
        1, 1, 2, 2, false);
    pshader->SetInput(0, "vertexTCoord", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    pshader->SetOutput(0, "pixelColor", Shader::VT_FLOAT4,
        Shader::VS_COLOR0);
    pshader->SetConstant(0, "Delta", 1);
    pshader->SetConstant(1, "Weight", 1);
    pshader->SetSampler(0, "OffsetSampler", Shader::ST_2D);
    pshader->SetSampler(1, "StateSampler", Shader::ST_2D);
    pshader->SetBaseRegisters(msPRegisters);
    pshader->SetTextureUnits(msPTextureUnits);
    pshader->SetPrograms(msPPrograms);
    return pshader;
}
//----------------------------------------------------------------------------
void GpuGaussianBlur3::SetBlurInput ()
{
    VisualEffectInstance* instance = mIP->GetMainEffectInstance();
    int bound0M1 = mIP->GetBound0() - 1;
    int bound1M1 = mIP->GetBound1() - 1;
    int bound2M1 = mIP->GetBound2() - 1;

    float dCol = mIP->GetColSpacing();
    float dRow = mIP->GetRowSpacing();

    ShaderFloat* deltaConstant = new0 ShaderFloat(1);
    float* delta = deltaConstant->GetData();
    delta[0] = dCol;
    delta[1] = 0.0f;
    delta[2] = 0.0f;
    delta[3] = dRow;
    instance->SetPixelConstant(0, "Delta", deltaConstant);

    ShaderFloat* weightConstant = new0 ShaderFloat(1);
    float* weight = weightConstant->GetData();
    weight[0] = 0.01f;  // = kappa*DeltaT/DeltaX^2
    weight[1] = 0.01f;  // = kappa*DeltaT/DeltaY^2
    weight[2] = 0.01f;  // = kappa*DeltaT/DeltaZ^2
    weight[3] = 1.0f - 2.0f*(weight[0] + weight[1] + weight[2]);  // w[3] > 0
    instance->SetPixelConstant(0, "Weight", weightConstant);

    Texture2D* offsetTexture = new Texture2D(Texture::TF_A32B32G32R32F,
        1024, 1024, 1);
    Float4* offset = (Float4*)offsetTexture->GetData(0);
    memset(offset, 0, 1024*1024*sizeof(Float4));

    // Interior voxels.  The offsets at the boundary are all zero, so the
    // finite differences are incorrect at those locations.  However, the
    // boundary effect will overwrite those voxels, so it is irrelevant
    // about the finite difference approximations at those locations.
    for (int z = 1; z < bound2M1; ++z)
    {
        for (int y = 1; y < bound1M1; ++y)
        {
            for (int x = 1; x < bound0M1; ++x)
            {
                // Get the 2D location of the voxel.
                int u, v;
                mIP->Map3Dto2D(x, y, z, u, v);

                // Get the 2D location of the z+ neighbor.
                int upos, vpos;
                mIP->Map3Dto2D(x, y, z+1, upos, vpos);

                // Get the 2D location of the z- neighbor.
                int uneg, vneg;
                mIP->Map3Dto2D(x, y, z-1, uneg, vneg);

                Float4& color = offset[mIP->Index(u, v)];
                color[0] = (upos - u)*dCol;
                color[1] = (vpos - v)*dRow;
                color[2] = (uneg - u)*dCol;
                color[3] = (vneg - v)*dRow;
            }
        }
    }

    instance->SetPixelTexture(0, "OffsetSampler", offsetTexture);
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// Profiles.
//----------------------------------------------------------------------------
int GpuGaussianBlur3::msAllPRegisters[2] = { 0, 1 };
int* GpuGaussianBlur3::msPRegisters[Shader::MAX_PROFILES] =
{
    0,
    msAllPRegisters,
    msAllPRegisters,
    msAllPRegisters,
    msAllPRegisters
};

int GpuGaussianBlur3::msAllPTextureUnits[2] = { 0, 1 };
int* GpuGaussianBlur3::msPTextureUnits[Shader::MAX_PROFILES] =
{
    0,
    msAllPTextureUnits,
    msAllPTextureUnits,
    msAllPTextureUnits,
    msAllPTextureUnits
};

std::string GpuGaussianBlur3::msPPrograms[Shader::MAX_PROFILES] =
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
    "texld r1, t0, s0\n"
    "texld r6, t0, s1\n"
    "add r4.xy, t0, -c0\n"
    "add r5.xy, t0, c0\n"
    "mov r0.y, r1.w\n"
    "mov r0.x, r1.z\n"
    "add r0.xy, t0, r0\n"
    "add r1.xy, t0, r1\n"
    "mov r2.y, c0.w\n"
    "mov r2.x, c0.z\n"
    "add r3.xy, t0, r2\n"
    "mov r2.y, -c0.w\n"
    "mov r2.x, -c0.z\n"
    "add r2.xy, t0, r2\n"
    "texld r0, r0, s1\n"
    "texld r1, r1, s1\n"
    "texld r5, r5, s1\n"
    "texld r4, r4, s1\n"
    "texld r2, r2, s1\n"
    "texld r3, r3, s1\n"
    "add r2, r3, r2\n"
    "mul r2, r2, c1.y\n"
    "add r3, r5, r4\n"
    "mad r2, r3, c1.x, r2\n"
    "add r0, r1, r0\n"
    "mad r0, r0, c1.z, r2\n"
    "mad r0, r6, c1.w, r0\n"
    "mov oC0, r0\n",

    // PP_PS_3_0
    "ps_3_0\n"
    "dcl_2d s0\n"
    "dcl_2d s1\n"
    "dcl_texcoord0 v0.xy\n"
    "add r1.xy, v0, -c0.zwzw\n"
    "add r0.xy, v0, c0.zwzw\n"
    "texld r1, r1, s1\n"
    "texld r0, r0, s1\n"
    "add r2, r0, r1\n"
    "add r1.xy, v0, -c0\n"
    "add r0.xy, v0, c0\n"
    "texld r0, r0, s1\n"
    "texld r1, r1, s1\n"
    "add r1, r0, r1\n"
    "texld r0, v0, s0\n"
    "mul r2, r2, c1.y\n"
    "mad r2, r1, c1.x, r2\n"
    "add r1.xy, v0, r0.zwzw\n"
    "add r0.xy, v0, r0\n"
    "texld r1, r1, s1\n"
    "texld r0, r0, s1\n"
    "add r0, r0, r1\n"
    "mad r1, r0, c1.z, r2\n"
    "texld r0, v0, s1\n"
    "mad oC0, r0, c1.w, r1\n",

    // PP_ARBFP1
    "!!ARBfp1.0\n"
    "PARAM c[2] = { program.local[0..1] };\n"
    "TEMP R0;\n"
    "TEMP R1;\n"
    "TEMP R2;\n"
    "ADD R0.zw, fragment.texcoord[0].xyxy, -c[0];\n"
    "TEX R1, R0.zwzw, texture[1], 2D;\n"
    "ADD R0.xy, fragment.texcoord[0], c[0].zwzw;\n"
    "TEX R0, R0, texture[1], 2D;\n"
    "ADD R2, R0, R1;\n"
    "ADD R0.zw, fragment.texcoord[0].xyxy, -c[0].xyxy;\n"
    "TEX R1, R0.zwzw, texture[1], 2D;\n"
    "ADD R0.xy, fragment.texcoord[0], c[0];\n"
    "TEX R0, R0, texture[1], 2D;\n"
    "ADD R1, R0, R1;\n"
    "TEX R0, fragment.texcoord[0], texture[0], 2D;\n"
    "MUL R2, R2, c[1].y;\n"
    "MAD R2, R1, c[1].x, R2;\n"
    "ADD R0.zw, fragment.texcoord[0].xyxy, R0;\n"
    "TEX R1, R0.zwzw, texture[1], 2D;\n"
    "ADD R0.xy, fragment.texcoord[0], R0;\n"
    "TEX R0, R0, texture[1], 2D;\n"
    "ADD R0, R0, R1;\n"
    "MAD R1, R0, c[1].z, R2;\n"
    "TEX R0, fragment.texcoord[0], texture[1], 2D;\n"
    "MAD result.color, R0, c[1].w, R1;\n"
    "END\n"
};
//----------------------------------------------------------------------------
