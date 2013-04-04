// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "DisplacementEffect.h"
#include "Wm5PVWMatrixConstant.h"
#include "Wm5VWMatrixConstant.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, VisualEffect, DisplacementEffect);
WM5_IMPLEMENT_STREAM(DisplacementEffect);
WM5_IMPLEMENT_FACTORY(DisplacementEffect);
WM5_IMPLEMENT_DEFAULT_NAMES(VisualEffect, DisplacementEffect);

//----------------------------------------------------------------------------
DisplacementEffect::DisplacementEffect ()
{
    VertexShader* vshader = new0 VertexShader("Wm5.DisplacementEffect",
        2, 2, 1, 1, false);
    vshader->SetInput(0, "modelPosition", Shader::VT_FLOAT3,
        Shader::VS_POSITION);
    vshader->SetInput(1, "modelTCoord", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    vshader->SetOutput(0, "clipPosition", Shader::VT_FLOAT4,
        Shader::VS_POSITION);
    vshader->SetOutput(1, "vertexTCoord", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    vshader->SetConstant(0, "PVWMatrix", 4);
    vshader->SetSampler(0, "HeightSampler", Shader::ST_2D);
    vshader->SetFilter(0, Shader::SF_LINEAR);
    vshader->SetCoordinate(0, 0, Shader::SC_CLAMP_EDGE);
    vshader->SetCoordinate(0, 1, Shader::SC_CLAMP_EDGE);
    vshader->SetBaseRegisters(msVRegisters);
    vshader->SetTextureUnits(msVTextureUnits);
    vshader->SetPrograms(msVPrograms);

    PixelShader* pshader = new0 PixelShader("Wm5.DisplacementEffect",
        1, 1, 0, 0, false);
    pshader->SetInput(0, "vertexTCoord", Shader::VT_FLOAT2,
        Shader::VS_TEXCOORD0);
    pshader->SetOutput(0, "pixelColor", Shader::VT_FLOAT4,
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
}
//----------------------------------------------------------------------------
DisplacementEffect::~DisplacementEffect ()
{
}
//----------------------------------------------------------------------------
VisualEffectInstance* DisplacementEffect::CreateInstance (
    Texture2D* displacementTexture)
{
    VisualEffectInstance* instance = new0 VisualEffectInstance(this, 0);
    instance->SetVertexConstant(0, 0, new0 PVWMatrixConstant());
    instance->SetVertexTexture(0, 0, displacementTexture);
    return instance;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
DisplacementEffect::DisplacementEffect (LoadConstructor value)
    :
    VisualEffect(value)
{
}
//----------------------------------------------------------------------------
void DisplacementEffect::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    VisualEffect::Load(source);

    WM5_END_DEBUG_STREAM_LOAD(DisplacementEffect, source);
}
//----------------------------------------------------------------------------
void DisplacementEffect::Link (InStream& source)
{
    VisualEffect::Link(source);
}
//----------------------------------------------------------------------------
void DisplacementEffect::PostLink ()
{
    VisualEffect::PostLink();

    VisualPass* pass = mTechniques[0]->GetPass(0);
    VertexShader* vshader = pass->GetVertexShader();
    PixelShader* pshader = pass->GetPixelShader();
    vshader->SetBaseRegisters(msVRegisters);
    vshader->SetTextureUnits(msVTextureUnits);
    vshader->SetPrograms(msVPrograms);
    pshader->SetPrograms(msPPrograms);
}
//----------------------------------------------------------------------------
bool DisplacementEffect::Register (OutStream& target) const
{
    return VisualEffect::Register(target);
}
//----------------------------------------------------------------------------
void DisplacementEffect::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    VisualEffect::Save(target);

    WM5_END_DEBUG_STREAM_SAVE(DisplacementEffect, target);
}
//----------------------------------------------------------------------------
int DisplacementEffect::GetStreamingSize () const
{
    return VisualEffect::GetStreamingSize();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Profiles.
//----------------------------------------------------------------------------
int DisplacementEffect::msAllVRegisters[1] = { 0 };
int* DisplacementEffect::msVRegisters[Shader::MAX_PROFILES] =
{
    0,
    0,
    0,
    msAllVRegisters,
    msAllVRegisters
};

int DisplacementEffect::msAllVTextureUnits[1] = { 0 };
int* DisplacementEffect::msVTextureUnits[Shader::MAX_PROFILES] =
{
    0,
    0,
    0,
    msAllVTextureUnits,
    msAllVTextureUnits
};

std::string DisplacementEffect::msVPrograms[Shader::MAX_PROFILES] =
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
    "def c4, 0.00000000, 1.00000000, 0, 0\n"
    "dcl_position0 v0\n"
    "dcl_texcoord0 v1\n"
    "dcl_2d s0\n"
    "mov r1.xy, v0\n"
    "mov r1.w, c4.y\n"
    "mov r0.z, c4.x\n"
    "mov r0.xy, v1\n"
    "texldl r0.x, r0.xyzz, s0\n"
    "mov r1.z, r0.x\n"
    "dp4 o0.w, r1, c3\n"
    "dp4 o0.z, r1, c2\n"
    "dp4 o0.y, r1, c1\n"
    "dp4 o0.x, r1, c0\n"
    "mov o1.x, r0\n",

    // VP_ARBVP1
    "!!ARBvp1.0\n"
    "OPTION NV_vertex_program3;\n"
    "PARAM c[5] = { program.local[0..3],\n"
    "		{ 1 } };\n"
    "TEMP R0;\n"
    "TEMP R1;\n"
    "TEMP RC, HC;\n"
    "BB1:\n"
    "TEX   R0.x, vertex.texcoord[0], texture[0], 2D;\n"
    "MOV   R1.xy, vertex.position;\n"
    "MOV   R1.w, c[4].x;\n"
    "MOV   R1.z, R0.x;\n"
    "DP4   result.position.w, R1, c[3];\n"
    "DP4   result.position.z, R1, c[2];\n"
    "DP4   result.position.y, R1, c[1];\n"
    "DP4   result.position.x, R1, c[0];\n"
    "MOV   result.texcoord[0].x, R0;\n"
    "END\n"
};

std::string DisplacementEffect::msPPrograms[Shader::MAX_PROFILES] =
{
    // PP_NONE
    "",

    // PP_PS_1_1
    "",

    // PP_PS_2_0
    "",

    // PP_PS_3_0
    "ps_3_0\n"
    "def c0, 1.00000000, 0, 0, 0\n"
    "dcl_texcoord0 v0.x\n"
    "mov oC0.xyz, v0.x\n"
    "mov oC0.w, c0.x\n",

    // PP_ARBFP1
    "!!ARBfp1.0\n"
    "OPTION NV_fragment_program2;\n"
    "PARAM c[1] = { { 1 } };\n"
    "TEMP RC;\n"
    "TEMP HC;\n"
    "OUTPUT oCol = result.color;\n"
    "MOVR  oCol.xyz, fragment.texcoord[0].x;\n"
    "MOVR  oCol.w, c[0].x;\n"
    "END\n"
};
//----------------------------------------------------------------------------
