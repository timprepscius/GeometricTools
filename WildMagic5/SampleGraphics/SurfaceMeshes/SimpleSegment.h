// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef SIMPLESEGMENT_H
#define SIMPLESEGMENT_H

#include "Wm5CurveSegment.h"

namespace Wm5
{

class SimpleSegment : public CurveSegment
{
    WM5_DECLARE_RTTI;
    WM5_DECLARE_NAMES;
    WM5_DECLARE_STREAM(SimpleSegment);

public:
    // Construction and destruction.
    SimpleSegment (float amplitude = 1.0f, float frequency = Mathf::TWO_PI,
        float height = 1.0f);

    virtual ~SimpleSegment ();

    // Member access.
    float GetAmplitude () const;
    float GetFrequency () const;
    float GetHeight () const;

    // Position and derivatives up to third order.
    virtual APoint P (float u) const;
    virtual AVector PU (float u) const;
    virtual AVector PUU (float u) const;
    virtual AVector PUUU (float u) const;

private:
    float mAmplitude, mFrequency, mHeight;
    float mAmplFreq, mAmplFreqFreq, mAmplFreqFreqFreq;
};

WM5_REGISTER_STREAM(SimpleSegment);
typedef Pointer0<SimpleSegment> SimpleSegmentPtr;

}

#endif
