// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "SimpleSegment.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, CurveSegment, SimpleSegment);
WM5_IMPLEMENT_STREAM(SimpleSegment);
WM5_IMPLEMENT_FACTORY(SimpleSegment);
WM5_IMPLEMENT_DEFAULT_NAMES(CurveSegment, SimpleSegment);

//----------------------------------------------------------------------------
SimpleSegment::SimpleSegment (float amplitude, float frequency, float height)
    :
    CurveSegment(0.0f, 1.0f),
    mAmplitude(amplitude),
    mFrequency(frequency),
    mHeight(height)
{
    mAmplFreq = mAmplitude*mFrequency;
    mAmplFreqFreq = mAmplFreq*mFrequency;
    mAmplFreqFreqFreq = mAmplFreqFreq*mFrequency;
}
//----------------------------------------------------------------------------
SimpleSegment::~SimpleSegment ()
{
}
//----------------------------------------------------------------------------
float SimpleSegment::GetAmplitude () const
{
    return mAmplitude;
}
//----------------------------------------------------------------------------
float SimpleSegment::GetFrequency () const
{
    return mFrequency;
}
//----------------------------------------------------------------------------
float SimpleSegment::GetHeight () const
{
    return mHeight;
}
//----------------------------------------------------------------------------
APoint SimpleSegment::P (float u) const
{
    // P(u) = (A*cos(F*u),A*sin(F*u),H*u)
    float angle = mFrequency*u;
    return APoint(
        mAmplitude*Mathf::Cos(angle),
        mAmplitude*Mathf::Sin(angle),
        mHeight*u);
}
//----------------------------------------------------------------------------
AVector SimpleSegment::PU (float u) const
{
    // P'(u) = (-A*F*sin(F*u),A*F*cos(F*u),H)
    float angle = mFrequency*u;
    return AVector(
        -mAmplFreq*Mathf::Sin(angle),
        mAmplFreq*Mathf::Cos(angle),
        mHeight);
}
//----------------------------------------------------------------------------
AVector SimpleSegment::PUU (float u) const
{
    // P"(u) = (-A*F*F*cos(F*u),-A*F*F*sin(F*u),0)
    float angle = mFrequency*u;
    return AVector(
        -mAmplFreqFreq*Mathf::Cos(angle),
        -mAmplFreqFreq*Mathf::Sin(angle),
        0.0f);
}
//----------------------------------------------------------------------------
AVector SimpleSegment::PUUU (float u) const
{
    // P"'(u) = (A*F*F*F*sin(F*u),-A*F*F*F*cos(F*u),0)
    float angle = mFrequency*u;
    return AVector(
        mAmplFreqFreqFreq*Mathf::Sin(angle),
        -mAmplFreqFreqFreq*Mathf::Cos(angle),
        0.0f);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
SimpleSegment::SimpleSegment (LoadConstructor value)
    :
    CurveSegment(value),
    mAmplitude(0.0f),
    mFrequency(0.0f),
    mHeight(0.0f),
    mAmplFreq(0.0f),
    mAmplFreqFreq(0.0f),
    mAmplFreqFreqFreq(0.0f)
{
}
//----------------------------------------------------------------------------
void SimpleSegment::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    CurveSegment::Load(source);

    source.Read(mAmplitude);
    source.Read(mFrequency);
    source.Read(mHeight);

    mAmplFreq = mAmplitude*mFrequency;
    mAmplFreqFreq = mAmplFreq*mFrequency;
    mAmplFreqFreqFreq = mAmplFreqFreq*mFrequency;

    WM5_END_DEBUG_STREAM_LOAD(SimpleSegment, source);
}
//----------------------------------------------------------------------------
void SimpleSegment::Link (InStream& source)
{
    CurveSegment::Link(source);
}
//----------------------------------------------------------------------------
void SimpleSegment::PostLink ()
{
    CurveSegment::PostLink();
}
//----------------------------------------------------------------------------
bool SimpleSegment::Register (OutStream& target) const
{
    return CurveSegment::Register(target);
}
//----------------------------------------------------------------------------
void SimpleSegment::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    CurveSegment::Save(target);

    target.Write(mAmplitude);
    target.Write(mFrequency);
    target.Write(mHeight);

    WM5_END_DEBUG_STREAM_SAVE(CurveSegment, target);
}
//----------------------------------------------------------------------------
int SimpleSegment::GetStreamingSize () const
{
    int size = CurveSegment::GetStreamingSize();
    size += sizeof(mAmplitude);
    size += sizeof(mFrequency);
    size += sizeof(mHeight);
    return size;
}
//----------------------------------------------------------------------------
