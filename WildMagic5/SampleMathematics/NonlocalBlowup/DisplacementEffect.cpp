// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

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
    vshader->SetOutput(1, "vertexTCoord0", Shader::VT_FLOAT3,
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
        1, 1, 0, 3, false);
    pshader->SetInput(0, "vertexTCoord0", Shader::VT_FLOAT3,
        Shader::VS_TEXCOORD0);
    pshader->SetOutput(0, "pixelColor", Shader::VT_FLOAT4,
        Shader::VS_COLOR0);
    pshader->SetSampler(0, "GridSampler", Shader::ST_2D);
    pshader->SetFilter(0, Shader::SF_LINEAR_LINEAR);
    pshader->SetCoordinate(0, 0, Shader::SC_CLAMP_EDGE);
    pshader->SetCoordinate(0, 1, Shader::SC_CLAMP_EDGE);
    pshader->SetSampler(1, "ColorSampler", Shader::ST_1D);
    pshader->SetFilter(1, Shader::SF_LINEAR);
    pshader->SetCoordinate(1, 0, Shader::SC_CLAMP_EDGE);
    pshader->SetCoordinate(1, 1, Shader::SC_CLAMP_EDGE);
    pshader->SetSampler(2, "DomainSampler", Shader::ST_2D);
    pshader->SetFilter(2, Shader::SF_LINEAR);
    pshader->SetCoordinate(2, 0, Shader::SC_CLAMP_EDGE);
    pshader->SetCoordinate(2, 1, Shader::SC_CLAMP_EDGE);
    pshader->SetTextureUnits(msPTextureUnits);
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
    Texture2D* displacementTexture, Texture2D* gridTexture,
    Texture1D* colorTexture, Texture2D* domainTexture)
{
    VisualEffectInstance* instance = new0 VisualEffectInstance(this, 0);
    instance->SetVertexConstant(0, 0, new0 PVWMatrixConstant());
    instance->SetVertexTexture(0, 0, displacementTexture);
    instance->SetPixelTexture(0, 0, gridTexture);
    instance->SetPixelTexture(0, 1, colorTexture);
    instance->SetPixelTexture(0, 2, domainTexture);
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
    0,
    msAllVRegisters
};

int DisplacementEffect::msAllVTextureUnits[1] = { 0 };
int* DisplacementEffect::msVTextureUnits[Shader::MAX_PROFILES] =
{
    0,
    0,
    0,
    0,
    msAllVTextureUnits
};

int DisplacementEffect::msAllPTextureUnits[3] = { 1, 2, 3 };
int* DisplacementEffect::msPTextureUnits[Shader::MAX_PROFILES] =
{
    0,
    0,
    0,
    0,
    msAllPTextureUnits
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
    "",

    // VP_ARBVP1
    "!!ARBvp1.0\n"
    "OPTION NV_vertex_program3;\n"
    "PARAM c[5] = { program.local[0..3],\n"
    "		{ 1 } };\n"
    "TEMP R0;\n"
    "TEMP R1;\n"
    "TEMP RC, HC;\n"
    "BB0:\n"
    "TEX   R0.x, vertex.texcoord[0], texture[0], 2D;\n"
    "MOV   R1.xy, vertex.position;\n"
    "MOV   R1.w, c[4].x;\n"
    "MOV   R1.z, R0.x;\n"
    "DP4   result.position.w, R1, c[3];\n"
    "DP4   result.position.z, R1, c[2];\n"
    "DP4   result.position.y, R1, c[1];\n"
    "DP4   result.position.x, R1, c[0];\n"
    "MOV   result.texcoord[0].xy, vertex.texcoord[0];\n"
    "MOV   result.texcoord[0].z, R0.x;\n"
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
    "",

    // PP_ARBFP1
    "!!ARBfp1.0\n"
    "OPTION NV_fragment_program2;\n"
    "PARAM c[2] = { { 0.0625 },\n"
    "		{ 0.40000001, 0.5, 0.60000002, 1 } };\n"
    "TEMP R0;\n"
    "TEMP R1;\n"
    "TEMP R2;\n"
    "TEMP RC;\n"
    "TEMP HC;\n"
    "OUTPUT oCol = result.color;\n"
    "MULR_SAT R0.x, fragment.texcoord[0].z, c[0];\n"
    "TEX   R1, R0, texture[2], 1D;\n"
    "TEX   R2, fragment.texcoord[0], texture[3], 2D;\n"
    "TEX   R0, fragment.texcoord[0], texture[1], 2D;\n"
    "MULR  R0, R2, R0;\n"
    "MULR  R0, R0, R1;\n"
    "MADR  R0, -R2, c[1], R0;\n"
    "ADDR  oCol, R0, c[1];\n"
    "END\n"
};
//----------------------------------------------------------------------------
