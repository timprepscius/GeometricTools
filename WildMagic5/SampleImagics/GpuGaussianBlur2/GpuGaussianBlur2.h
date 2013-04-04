// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef GPUGAUSSIANBLUR2_H
#define GPUGAUSSIANBLUR2_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class GpuGaussianBlur2 : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    GpuGaussianBlur2 ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();

protected:
    PixelShader* CreateBlurPixelShader ();
    void SetBlurInput ();

    ImageProcessing2* mIP;
    Float4 mTextColor;

    // The profile information for GaussianBlur2.fx.
    static int msAllPRegisters[2];
    static int* msPRegisters[Shader::MAX_PROFILES];
    static int msAllPTextureUnits[1];
    static int* msPTextureUnits[Shader::MAX_PROFILES];
    static std::string msPPrograms[Shader::MAX_PROFILES];
};

WM5_REGISTER_INITIALIZE(GpuGaussianBlur2);
WM5_REGISTER_TERMINATE(GpuGaussianBlur2);

#endif

