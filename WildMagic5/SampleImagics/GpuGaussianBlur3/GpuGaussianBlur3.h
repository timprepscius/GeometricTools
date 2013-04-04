// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef GPUGAUSSIANBLUR3_H
#define GPUGAUSSIANBLUR3_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class GpuGaussianBlur3 : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    GpuGaussianBlur3 ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();

protected:
    PixelShader* CreateBlurPixelShader ();
    void SetBlurInput ();

    ImageProcessing3* mIP;
    Float4 mTextColor;

    // The profile information for GaussianBlur3.fx.
    static int msAllPRegisters[2];
    static int* msPRegisters[Shader::MAX_PROFILES];
    static int msAllPTextureUnits[2];
    static int* msPTextureUnits[Shader::MAX_PROFILES];
    static std::string msPPrograms[Shader::MAX_PROFILES];
};

WM5_REGISTER_INITIALIZE(GpuGaussianBlur3);
WM5_REGISTER_TERMINATE(GpuGaussianBlur3);

#endif

