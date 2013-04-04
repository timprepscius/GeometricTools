// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef SIMPLEPATCH_H
#define SIMPLEPATCH_H

#include "Wm5SurfacePatch.h"

namespace Wm5
{

class SimplePatch : public SurfacePatch
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(SimplePatch);

public:
    // Construction and destruction.
    SimplePatch (float amplitude = 0.0f);
    virtual ~SimplePatch ();

    // Member access.
    void SetAmplitude (float amplitude);
    float GetAmplitude () const;

    // Position and derivatives up to second order.
    virtual APoint P (float u, float v) const;
    virtual AVector PU (float u, float v) const;
    virtual AVector PV (float u, float v) const;
    virtual AVector PUU (float u, float v) const;
    virtual AVector PUV (float u, float v) const;
    virtual AVector PVV (float u, float v) const;

private:
    float mAmplitude;
};

WM5_REGISTER_STREAM(SimplePatch);
typedef Pointer0<SimplePatch> SimplePatchPtr;

}

#endif
