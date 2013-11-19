// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.2.0 (2010/07/31)

#include "RootFinderEffect.h"
#include "Wm5PVWMatrixConstant.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, VisualEffect, RootFinderEffect);
WM5_IMPLEMENT_STREAM(RootFinderEffect);
WM5_IMPLEMENT_FACTORY(RootFinderEffect);
WM5_IMPLEMENT_DEFAULT_NAMES(VisualEffect, RootFinderEffect);
WM5_IMPLEMENT_DEFAULT_STREAM(VisualEffect, RootFinderEffect);

//----------------------------------------------------------------------------
RootFinderEffect::RootFinderEffect ()
{
    VertexShader* vshader = new0 VertexShader("RootFinderEffect",
        2, 2, 1, 0, false);
    vshader->SetInput(0, "modelPosition", Shader::VT_FLOAT3,
        Shader::VS_POSITION);
    vshader->SetInput(1, "modelTCoord", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    vshader->SetOutput(0, "clipPosition", Shader::VT_FLOAT4,
        Shader::VS_POSITION);
    vshader->SetOutput(1, "vertexTCoord", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    vshader->SetConstant(0, "PVWMatrix", 4);
    vshader->SetBaseRegisters(msVRegisters);
    vshader->SetPrograms(msVPrograms);

    PixelShader* pshader;
    if (VertexShader::GetProfile() == VertexShader::VP_ARBVP1)
    {
        pshader = new0 PixelShader("RootFinderEffect", 1, 1, 0, 0, false);
    }
    else
    {
        pshader = new0 PixelShader("RootFinderEffect", 1, 1, 0, 1, false);
        pshader->SetSampler(0, "TrailingSampler", Shader::ST_2D);
        pshader->SetFilter(0, Shader::SF_NEAREST);
        pshader->SetCoordinate(0, 0, Shader::SC_CLAMP_EDGE);
        pshader->SetCoordinate(0, 1, Shader::SC_CLAMP_EDGE);
        pshader->SetTextureUnits(msPTextureUnits);

        mTrailing = new0 Texture2D(Texture::TF_G32R32F, 4096, 2048, 1);
        float* data = (float*)mTrailing->GetData(0);
        union { uint32_t encoding; float number; }
        curr = { 0x3F800000u }, next = { 0x3F800001u };
        for (int i = 0; i < 4096*2048; ++i, ++curr.encoding, ++next.encoding)
        {
            *data++ = curr.number;
            *data++ = next.number;
        }
    }
    pshader->SetInput(0, "vertexTCoord", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    pshader->SetOutput(0, "pixelColor0", Shader::VT_FLOAT4,
        Shader::VS_COLOR0);
    pshader->SetPrograms(msPPrograms);

    VisualPass* pass = new0 VisualPass();
    pass->SetVertexShader(vshader);
    pass->SetPixelShader(pshader);
    pass->SetAlphaState(new0 AlphaState());
    pass->SetCullState(new0 CullState());
    pass->SetDepthState(new0 DepthState());
    pass->SetOffsetState(new0 OffsetState());
    pass->SetStencilState(new0 StencilState());
    pass->SetWireState(new0 WireState());

    VisualTechnique* technique = new0 VisualTechnique();
    technique->InsertPass(pass);
    InsertTechnique(technique);

    // Culling is not needed.
    GetCullState(0, 0)->Enabled = false;

    // Depth buffering is not needed.
    GetDepthState(0, 0)->Enabled = false;
    GetDepthState(0, 0)->Writable = false;
}
//----------------------------------------------------------------------------
RootFinderEffect::~RootFinderEffect ()
{
}
//----------------------------------------------------------------------------
VisualEffectInstance* RootFinderEffect::CreateInstance ()
{
    VisualEffectInstance* instance = new0 VisualEffectInstance(this, 0);
    instance->SetVertexConstant(0, 0, new0 PVWMatrixConstant());
    if (VertexShader::GetProfile() != VertexShader::VP_ARBVP1)
    {
        instance->SetPixelTexture(0, 0, mTrailing);
    }
    return instance;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Vertex and pixel programs.
//----------------------------------------------------------------------------
int RootFinderEffect::msDx9VRegisters[1] = { 0 };
int RootFinderEffect::msOglVRegisters[1] = { 0 };
int* RootFinderEffect::msVRegisters[Shader::MAX_PROFILES] =
{
    0,
    0,
    0,
    msDx9VRegisters,
    msOglVRegisters
};

std::string RootFinderEffect::msVPrograms[Shader::MAX_PROFILES] =
{
    // VP_NONE
    "",

    // VP_VS_1_1
    "",

    // VP_VS_2_0
    "",

    // VP_VS_3_0
    "vs_3_0\n"
    "dcl_position o0\n"
    "dcl_texcoord0 o1\n"
    "def c4, 1.00000000, 0, 0, 0\n"
    "dcl_position0 v0\n"
    "dcl_texcoord0 v1\n"
    "mov r0.w, c4.x\n"
    "mov r0.xyz, v0\n"
    "dp4 o0.w, r0, c3\n"
    "dp4 o0.z, r0, c2\n"
    "dp4 o0.y, r0, c1\n"
    "dp4 o0.x, r0, c0\n"
    "mov o1.xy, v1\n",

    // VP_ARBVP1
    "!!ARBvp1.0\n"
    "OPTION NV_vertex_program3;\n"
    "PARAM c[5] = { program.local[0..3],\n"
    "		{ 1 } };\n"
    "TEMP R0;\n"
    "TEMP RC, HC;\n"
    "BB1:\n"
    "MOV   R0.w, c[4].x;\n"
    "MOV   R0.xyz, vertex.position;\n"
    "DP4   result.position.w, R0, c[3];\n"
    "DP4   result.position.z, R0, c[2];\n"
    "DP4   result.position.y, R0, c[1];\n"
    "DP4   result.position.x, R0, c[0];\n"
    "MOV   result.texcoord[0].xy, vertex.texcoord[0];\n"
    "END\n"
};

int RootFinderEffect::msAllPTextureUnits[1] = { 0 };
int* RootFinderEffect::msPTextureUnits[Shader::MAX_PROFILES] =
{
    0,
    0,
    0,
    msAllPTextureUnits,
    msAllPTextureUnits
};

std::string RootFinderEffect::msPPrograms[Shader::MAX_PROFILES] =
{
    // PP_NONE
    "",

    // PP_PS_1_1
    "",

    // PP_PS_2_0
    "",

    // PP_PS_3_0
    "ps_3_0\n"
    "dcl_2d s0\n"
    "def c0, 1.00000000, 0.00000000, -127.00000000, -1.10000002\n"
    "defi i0, 254, 1, 1, 0\n"
    "def c1, 2.20000005, 0.00392157, 0, 0\n"
    "dcl_texcoord0 v0.xy\n"
    "texld r0.xy, v0, s0\n"
    "mov r1, c0.xyxy\n"
    "mov oC0, c0.xyxy\n"
    "mov r2.xy, r0\n"
    "mov r2.z, c0.x\n"
    "loop aL, i0\n"
    "add r0.x, r2.z, c0.z\n"
    "abs r0.y, r0.x\n"
    "frc r0.z, r0.y\n"
    "add r0.y, r0, -r0.z\n"
    "cmp r0.x, r0, r0.y, -r0.y\n"
    "exp_pp r0.w, r0.x\n"
    "mad r0.y, r2, r0.w, c1.x\n"
    "mad r0.x, r2.y, r0.w, c0.w\n"
    "mul r0.x, r0, r0.y\n"
    "mad r0.z, r2.x, r0.w, c1.x\n"
    "mad r0.y, r2.x, r0.w, c0.w\n"
    "mul r0.y, r0, r0.z\n"
    "mov r0.z, r0.x\n"
    "mov r0.x, -r0\n"
    "cmp r0.z, r0, c0.y, c0.x\n"
    "cmp r0.x, r0, c0.y, c0\n"
    "add r0.x, r0, -r0.z\n"
    "mov r0.z, r0.y\n"
    "mov r0.y, -r0\n"
    "cmp r0.z, r0, c0.y, c0.x\n"
    "cmp r0.y, r0, c0, c0.x\n"
    "add r0.y, r0, -r0.z\n"
    "mov r0.z, r0.x\n"
    "mov r0.x, -r0\n"
    "cmp r0.z, r0, c0.y, c0.x\n"
    "cmp r0.x, r0, c0.y, c0\n"
    "add r0.x, r0, -r0.z\n"
    "mov r0.z, r0.y\n"
    "mov r0.y, -r0\n"
    "cmp r0.z, r0, c0.y, c0.x\n"
    "cmp r0.y, r0, c0, c0.x\n"
    "add r0.y, r0, -r0.z\n"
    "mul r0.z, r2, c1.y\n"
    "mul r0.x, r0.y, r0\n"
    "mov r0.x, -r0\n"
    "min r2.w, r1.x, r0.z\n"
    "cmp r0.y, r0.x, c0.x, c0\n"
    "cmp r0.x, -r0.y, r1, r2.w\n"
    "mad r2.w, -r2.y, r0, c1.x\n"
    "mad r1.x, -r2.y, r0.w, c0.w\n"
    "mul r1.x, r1, r2.w\n"
    "mad r2.w, -r2.x, r0, c1.x\n"
    "mad r0.w, -r2.x, r0, c0\n"
    "mul r0.w, r0, r2\n"
    "mov r2.w, r1.x\n"
    "mov r1.x, -r1\n"
    "cmp r2.w, r2, c0.y, c0.x\n"
    "cmp r1.x, r1, c0.y, c0\n"
    "add r1.x, r1, -r2.w\n"
    "mov r2.w, r0\n"
    "mov r0.w, -r0\n"
    "cmp r2.w, r2, c0.y, c0.x\n"
    "cmp r0.w, r0, c0.y, c0.x\n"
    "add r0.w, r0, -r2\n"
    "mov r2.w, r1.x\n"
    "mov r1.x, -r1\n"
    "cmp r1.x, r1, c0.y, c0\n"
    "cmp r2.w, r2, c0.y, c0.x\n"
    "add r2.w, r1.x, -r2\n"
    "mov r1.x, r0.w\n"
    "mov r0.w, -r0\n"
    "cmp r1.x, r1, c0.y, c0\n"
    "cmp r0.w, r0, c0.y, c0.x\n"
    "add r0.w, r0, -r1.x\n"
    "max r1.x, r1.y, r0.z\n"
    "mul r0.w, r0, r2\n"
    "cmp r0.y, -r0, r1, r1.x\n"
    "mov r0.w, -r0\n"
    "cmp r1.x, r0.w, c0, c0.y\n"
    "max r0.w, r1, r0.z\n"
    "min r0.z, r1, r0\n"
    "cmp r0.w, -r1.x, r1, r0\n"
    "cmp r0.z, -r1.x, r1, r0\n"
    "mov r1, r0\n"
    "mov oC0, r0\n"
    "add r2.z, r2, c0.x\n"
    "endloop\n",

    // PP_ARBFP1
    "!!ARBfp1.0\n"
    "OPTION NV_fragment_program2;\n"
    "OPTION ARB_precision_hint_nicest;\n"
    "PARAM c[4] = { { 2048, 0.5, 4096, 1.1920929e-007 },\n"
    "		{ 1, 0, 127, 1.1 },\n"
    "		{ 254, 1, 1, 2.2 },\n"
    "		{ 0.0039215689 } };\n"
    "TEMP R0;\n"
    "TEMP R1;\n"
    "TEMP R2;\n"
    "TEMP R3;\n"
    "SHORT TEMP H0;\n"
    "TEMP RC;\n"
    "TEMP HC;\n"
    "OUTPUT oCol = result.color;\n"
    "MADR  R1.xy, fragment.texcoord[0].yxzw, c[0].xzzw, -c[0].y;\n"
    "MADR  R1.z, R1.x, c[0], R1.y;\n"
    "MOVR  R1.x, c[1];\n"
    "ADDR  R1.y, R1.z, c[1].x;\n"
    "MOVR  R0, c[1].xyxy;\n"
    "MOVR  oCol, c[1].xyxy;\n"
    "MADR  R2.x, R1.z, c[0].w, R1;\n"
    "MADR  R2.y, R1, c[0].w, R1.x;\n"
    "MOVR  R2.z, c[1].x;\n"
    "LOOP c[2];\n"
    "ADDR  R1.x, R2.z, -c[1].z;\n"
    "FLRR  H0.y, |R1.x|;\n"
    "MULR  R2.w, R2.z, c[3].x;\n"
    "SLTRC HC.x, R2.z, c[1].z;\n"
    "MOVH  H0.x, H0.y;\n"
    "MOVH  H0.x(GT), -H0.y;\n"
    "EX2H  H0.x, H0.x;\n"
    "MADR  R1.y, R2.x, H0.x, c[2].w;\n"
    "MADR  R1.x, R2, H0, -c[1].w;\n"
    "MULR  R1.x, R1, R1.y;\n"
    "SLTR  H0.z, R1.x, c[1].y;\n"
    "SGTR  H0.y, R1.x, c[1];\n"
    "SLTR  H0.w, H0.y, H0.z;\n"
    "SGTR  H0.y, H0, H0.z;\n"
    "ADDR  R1.y, H0, -H0.w;\n"
    "MADR  R1.z, H0.x, R2.y, c[2].w;\n"
    "MADR  R1.x, H0, R2.y, -c[1].w;\n"
    "MULR  R1.x, R1, R1.z;\n"
    "MINR  R1.zw, R0.xyxz, R2.w;\n"
    "SLTR  H0.z, R1.x, c[1].y;\n"
    "SGTR  H0.y, R1.x, c[1];\n"
    "SLTR  H0.w, H0.y, H0.z;\n"
    "SGTR  H0.y, H0, H0.z;\n"
    "ADDR  R3.x, H0.y, -H0.w;\n"
    "MULRC HC.x, R1.y, R3;\n"
    "MOVR  R1.x, R0;\n"
    "MOVR  R1.x(LE), R1.z;\n"
    "MOVR  R1.y, R0;\n"
    "MAXR  R0.xy, R0.ywzw, R2.w;\n"
    "MOVR  R1.y(LE.x), R0.x;\n"
    "MADR  R1.z, -R2.x, H0.x, c[2].w;\n"
    "MADR  R0.x, -R2, H0, -c[1].w;\n"
    "MULR  R0.x, R0, R1.z;\n"
    "SLTR  H0.z, R0.x, c[1].y;\n"
    "SGTR  H0.y, R0.x, c[1];\n"
    "SLTR  H0.w, H0.y, H0.z;\n"
    "SGTR  H0.y, H0, H0.z;\n"
    "MADR  R1.z, H0.x, -R2.y, c[2].w;\n"
    "MADR  R0.x, H0, -R2.y, -c[1].w;\n"
    "MULR  R0.x, R0, R1.z;\n"
    "ADDR  R2.w, H0.y, -H0;\n"
    "SLTR  H0.y, R0.x, c[1];\n"
    "SGTR  H0.x, R0, c[1].y;\n"
    "SLTR  H0.z, H0.x, H0.y;\n"
    "SGTR  H0.x, H0, H0.y;\n"
    "ADDR  R0.x, H0, -H0.z;\n"
    "MULRC HC.x, R2.w, R0;\n"
    "MOVR  R0.x, R0.z;\n"
    "MOVR  R0.x(LE), R1.w;\n"
    "MOVR  R0.w(LE.x), R0.y;\n"
    "MOVR  R1.w, R0;\n"
    "MOVR  R1.z, R0.x;\n"
    "MOVR  R0, R1;\n"
    "MOVR  oCol, R1;\n"
    "ADDR  R2.z, R2, c[1].x;\n"
    "ENDLOOP;\n"
    "END\n"
};
//----------------------------------------------------------------------------
