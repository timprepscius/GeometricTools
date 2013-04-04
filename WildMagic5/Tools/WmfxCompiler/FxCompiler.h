// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/04/14)

#ifndef FXCOMPILER_H
#define FXCOMPILER_H

#include "Wm5VisualEffect.h"
using namespace Wm5;

class FxCompiler
{
public:
    // Construction and destruction.  There are times that special profiles
    // are needed that require running cgc manually to produce the compiled
    // shaders.  In this case, 'alreadyCompiled' should be set to 'true'
    // so that FxCompiler skips the compilation phase.  It will still load
    // the compiled files and create a *.wmfx file.
    FxCompiler (const std::string& fxName, bool alreadyCompiled = false);
    ~FxCompiler ();

    std::vector<std::string> Messages;

private:
    typedef std::vector<std::string> TokenArray;
    typedef std::vector<TokenArray> TokenArrays;

    class Program
    {
    public:
        Program ();

        std::string Name;
        std::string Text;
        TokenArrays Variables;
    };

    class Input
    {
    public:
        std::string Name;
        Shader::VariableType Type;
        Shader::VariableSemantic Semantic;
    };
    typedef std::vector<Input> InputArray;

    class Output
    {
    public:
        std::string Name;
        Shader::VariableType Type;
        Shader::VariableSemantic Semantic;
    };
    typedef std::vector<Output> OutputArray;

    class Constant
    {
    public:
        std::string Name;
        Shader::VariableType Type;
        int BaseRegister;
        int NumRegistersUsed;
    };
    typedef std::vector<Constant> ConstantArray;

    class Sampler
    {
    public:
        std::string Name;
        Shader::SamplerType Type;
        int Unit;
        Shader::SamplerFilter Filter;
        Shader::SamplerCoordinate Coordinate[3];
        float LodBias;
        float Anisotropy;
        Float4 BorderColor;
    };
    typedef std::vector<Sampler> SamplerArray;

    void InitializeMaps ();

    bool Parse (const std::string& fileName, const std::string& profileName,
        Program& program);

    void GetTokens (const std::string& line, std::string::size_type begin,
        std::vector<std::string>& tokens);

    bool CreateEffect (const Program& vProgram, const Program& pProgram);
    bool UpdateEffect (const Program& vProgram, const Program& pProgram);

    bool Process (const Program& program, InputArray& inputs,
        OutputArray& outputs, ConstantArray& constants,
        SamplerArray& samplers);

    bool GetInput (const TokenArray& tokens, const std::string& name,
        Shader::VariableType type, InputArray& inputs);

    bool GetOutput (const TokenArray& tokens, const std::string& name,
        Shader::VariableType type, OutputArray& outputs);

    bool GetConstant (const TokenArray& tokens, const std::string& name,
        Shader::VariableType type, ConstantArray& constants);

    bool GetSampler (const TokenArray& tokens, const std::string& name,
        Shader::SamplerType type, SamplerArray& samplers);

    Shader* CreateShader (bool isVShader, const Program& program,
        InputArray& inputs, OutputArray& outputs, ConstantArray& constants,
        SamplerArray& samplers);

    bool UpdateShader (Shader* shader, const Program& program,
        InputArray& inputs, OutputArray& outputs, ConstantArray& constants,
        SamplerArray& samplers);

    void ReportError (const std::string& message,
        const TokenArray* tokens = 0);

    typedef std::map<std::string, Shader::VariableType> VariableTypeMap;
    VariableTypeMap mVariableTypes;

    typedef std::map<std::string, Shader::VariableSemantic> SemanticMap;
    SemanticMap mSemantics;

    typedef std::map<std::string, Shader::SamplerType> SamplerTypeMap;
    SamplerTypeMap mSamplerTypes;

    int mActiveProfile;
    VisualEffect* mEffect;
    VertexShader* mVShader;
    PixelShader* mPShader;
    bool mAlreadyCompiled;

    static const std::string msVProfileName[Shader::MAX_PROFILES];
    static const std::string msPProfileName[Shader::MAX_PROFILES];
    static const std::string msVTName[Shader::VT_QUANTITY];
    static const std::string msVSName[Shader::VS_QUANTITY];
    static const std::string msSTName[Shader::ST_QUANTITY];
};

#endif
