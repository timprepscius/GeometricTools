// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "GpuGaussianBlur2.h"

WM5_WINDOW_APPLICATION(GpuGaussianBlur2);

//----------------------------------------------------------------------------
GpuGaussianBlur2::GpuGaussianBlur2 ()
    :
    WindowApplication3("SampleImagics/GpuGaussianBlur2", 0, 0, 256, 256,
        Float4(1.0f, 0.0f, 0.0f, 0.0f)),
        mTextColor(1.0f, 1.0f, 0.0f, 1.0f)
{
    Environment::InsertDirectory(ThePath + "Data/");

    mIP = 0;
}
//----------------------------------------------------------------------------
bool GpuGaussianBlur2::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    bool useDirichlet = false;
    if (TheCommand)
    {
        // usage for Dirichlet:  GpuGaussianBlur2 -d
        // usage for Neumann:    GpuGaussianBlur2
        useDirichlet = TheCommand->GetBoolean("d") != 0;
    }

    // Load the image to be blurred.  The image is known to be 256x256.
#ifdef WM5_LITTLE_ENDIAN
    std::string imageName = Environment::GetPathR(
        "Head.x256y256.short.le.raw");
#else
    std::string imageName = Environment::GetPathR(
        "Head.x256y256.short.be.raw");
#endif
    assertion(imageName != "", "Cannot find or open image file\n");
    FILE* inFile = fopen(imageName.c_str(), "rb");
    int numPixels = 256*256;
    short* rawData = new1<short>(numPixels);
    fread(rawData, sizeof(short), numPixels, inFile);
    fclose(inFile);

    // For the test image, smin = 0 and smax = 1024.  Let us compute them
    // anyway (you would do this generally for any input image).
    short smin = rawData[0], smax = rawData[0];
    int i;
    for (int i = 1; i < numPixels; ++i)
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

    Float4* imageData = new1<Float4>(numPixels);
    for (i = 0; i < numPixels; ++i)
    {
        float gray = ((float)rawData[i] - trn)*mult;
        imageData[i] = Float4(gray, gray, gray, gray);
    }
    delete1(rawData);

    // Create the screen objects for image processing.
    mIP = new0 ImageProcessing2(256, 256, imageData,
        CreateBlurPixelShader(), useDirichlet);

    delete1(imageData);
    SetBlurInput();
    mIP->Initialize(mRenderer);
    return true;
}
//----------------------------------------------------------------------------
void GpuGaussianBlur2::OnTerminate ()
{
    delete0(mIP);
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void GpuGaussianBlur2::OnIdle ()
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
PixelShader* GpuGaussianBlur2::CreateBlurPixelShader ()
{
    PixelShader* pshader = new0 PixelShader("Wm5.GaussianBlur2",
        1, 1, 2, 1, false);
    pshader->SetInput(0, "vertexTCoord", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    pshader->SetOutput(0, "pixelColor", Shader::VT_FLOAT4,
        Shader::VS_COLOR0);
    pshader->SetConstant(0, "Delta", 1);
    pshader->SetConstant(1, "Weight", 1);
    pshader->SetSampler(0, "StateSampler", Shader::ST_2D);
    pshader->SetBaseRegisters(msPRegisters);
    pshader->SetTextureUnits(msPTextureUnits);
    pshader->SetPrograms(msPPrograms);
    return pshader;
}
//----------------------------------------------------------------------------
void GpuGaussianBlur2::SetBlurInput ()
{
    VisualEffectInstance* instance = mIP->GetMainEffectInstance();
    float dCol = mIP->GetColSpacing();
    float dRow = mIP->GetRowSpacing();

    ShaderFloat* deltaConstant = new0 ShaderFloat(1);
    float* delta = deltaConstant->GetData();
    delta[0] = dCol;
    delta[1] = dRow;
    instance->SetPixelConstant(0, "Delta", deltaConstant);

    ShaderFloat* weightConstant = new0 ShaderFloat(1);
    float* weight = weightConstant->GetData();
    weight[0] = 0.01f;  // = kappa*DeltaT/DeltaX^2
    weight[1] = 0.01f;  // = kappa*DeltaT/DeltaY^2
    weight[2] = 1.0f - 2.0f*weight[0] - 2.0f*weight[1];  // must be positive
    instance->SetPixelConstant(0, "Weight", weightConstant);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Profiles.
//----------------------------------------------------------------------------
int GpuGaussianBlur2::msAllPRegisters[2] = { 0, 1 };
int* GpuGaussianBlur2::msPRegisters[Shader::MAX_PROFILES] =
{
    0,
    msAllPRegisters,
    msAllPRegisters,
    msAllPRegisters,
    msAllPRegisters
};

int GpuGaussianBlur2::msAllPTextureUnits[1] = { 0 };
int* GpuGaussianBlur2::msPTextureUnits[Shader::MAX_PROFILES] =
{
    0,
    msAllPTextureUnits,
    msAllPTextureUnits,
    msAllPTextureUnits,
    msAllPTextureUnits
};

std::string GpuGaussianBlur2::msPPrograms[Shader::MAX_PROFILES] =
{
    // PP_NONE
    "",

    // PP_PS_1_1
    "",

    // PP_PS_2_0
    "ps_2_0\n"
    "dcl_2d s0\n"
    "dcl t0.xy\n"
    "texld r4, t0, s0\n"
    "add r1.xy, t0, c0\n"
    "add r0.xy, t0, -c0\n"
    "mov r3.x, r1\n"
    "mov r2.x, r0\n"
    "mov r3.y, t0\n"
    "mov r2.y, t0\n"
    "mov r0.x, t0\n"
    "mov r1.x, t0\n"
    "texld r1, r1, s0\n"
    "texld r0, r0, s0\n"
    "texld r2, r2, s0\n"
    "texld r3, r3, s0\n"
    "add r2, r3, r2\n"
    "mul r2, r2, c1.x\n"
    "mad r2, r4, c1.z, r2\n"
    "add r0, r1, r0\n"
    "mad r0, r0, c1.y, r2\n"
    "mov oC0, r0\n",

    // PP_PS_3_0
    "ps_3_0\n"
    "dcl_2d s0\n"
    "dcl_texcoord0 v0.xy\n"
    "add r0.xy, v0, -c0\n"
    "add r0.zw, v0.xyxy, c0.xyxy\n"
    "mov r1.x, r0\n"
    "mov r1.y, v0\n"
    "mov r2.x, r0.z\n"
    "mov r2.y, v0\n"
    "texld r2, r2, s0\n"
    "texld r1, r1, s0\n"
    "add r1, r2, r1\n"
    "mul r2, r1, c1.x\n"
    "texld r1, v0, s0\n"
    "mad r1, r1, c1.z, r2\n"
    "mov r0.x, v0\n"
    "texld r2, r0, s0\n"
    "mov r0.y, r0.w\n"
    "mov r0.x, v0\n"
    "texld r0, r0, s0\n"
    "add r0, r0, r2\n"
    "mad oC0, r0, c1.y, r1\n",

    // PP_ARBFP1
    "!!ARBfp1.0\n"
    "PARAM c[2] = { program.local[0..1] };\n"
    "TEMP R0;\n"
    "TEMP R1;\n"
    "TEMP R2;\n"
    "ADD R0.zw, fragment.texcoord[0].xyxy, -c[0].xyxy;\n"
    "MOV R0.x, R0.z;\n"
    "MOV R0.y, fragment.texcoord[0];\n"
    "TEX R1, R0, texture[0], 2D;\n"
    "ADD R0.xy, fragment.texcoord[0], c[0];\n"
    "MOV R2.x, R0;\n"
    "MOV R2.y, fragment.texcoord[0];\n"
    "TEX R2, R2, texture[0], 2D;\n"
    "ADD R1, R2, R1;\n"
    "MUL R2, R1, c[1].x;\n"
    "TEX R1, fragment.texcoord[0], texture[0], 2D;\n"
    "MAD R1, R1, c[1].z, R2;\n"
    "MOV R0.z, fragment.texcoord[0].x;\n"
    "TEX R2, R0.zwzw, texture[0], 2D;\n"
    "MOV R0.x, fragment.texcoord[0];\n"
    "TEX R0, R0, texture[0], 2D;\n"
    "ADD R0, R0, R2;\n"
    "MAD result.color, R0, c[1].y, R1;\n"
    "END\n"
};
//----------------------------------------------------------------------------
