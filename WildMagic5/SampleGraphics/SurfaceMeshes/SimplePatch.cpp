// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2012/07/04)

#include "SimplePatch.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, SurfacePatch, SimplePatch);
WM5_IMPLEMENT_STREAM(SimplePatch);
WM5_IMPLEMENT_FACTORY(SimplePatch);
WM5_IMPLEMENT_DEFAULT_NAMES(SurfacePatch, SimplePatch);

//----------------------------------------------------------------------------
SimplePatch::SimplePatch (float)
    :
    SurfacePatch(-2.0f, 2.0f, -2.0f, 2.0f, true),
    mAmplitude(0.0f)
{
}
//----------------------------------------------------------------------------
SimplePatch::~SimplePatch ()
{
}
//----------------------------------------------------------------------------
void SimplePatch::SetAmplitude (float amplitude)
{
    mAmplitude = amplitude;
}
//----------------------------------------------------------------------------
float SimplePatch::GetAmplitude () const
{
    return mAmplitude;
}
//----------------------------------------------------------------------------
APoint SimplePatch::P (float u, float v) const
{
    // P(u,v) = (u,v,A*u*v)
    return APoint(u, v, mAmplitude*u*v);
}
//----------------------------------------------------------------------------
AVector SimplePatch::PU (float, float v) const
{
    // P_u = (1,0,A*v)
    return AVector(1.0f, 0.0f, mAmplitude*v);
}
//----------------------------------------------------------------------------
AVector SimplePatch::PV (float u, float) const
{
    // P_v = (0,1,A*u)
    return AVector(0.0f, 1.0f, mAmplitude*u);
}
//----------------------------------------------------------------------------
AVector SimplePatch::PUU (float, float) const
{
    // P_uu = (0,0,0)
    return AVector(0.0f, 0.0f, 0.0f);
}
//----------------------------------------------------------------------------
AVector SimplePatch::PUV (float, float) const
{
    // P_uv = (0,0,A)
    return AVector(0.0f, 0.0f, mAmplitude);
}
//----------------------------------------------------------------------------
AVector SimplePatch::PVV (float, float) const
{
    // P_vv = (0,0,0)
    return AVector(0.0f, 0.0f, 0.0f);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
SimplePatch::SimplePatch (LoadConstructor value)
    :
    SurfacePatch(value),
    mAmplitude(0.0f)
{
}
//----------------------------------------------------------------------------
void SimplePatch::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    SurfacePatch::Load(source);

    source.Read(mAmplitude);

    WM5_END_DEBUG_STREAM_LOAD(SimplePatch, source);
}
//----------------------------------------------------------------------------
void SimplePatch::Link (InStream& source)
{
    SurfacePatch::Link(source);
}
//----------------------------------------------------------------------------
void SimplePatch::PostLink ()
{
    SurfacePatch::PostLink();
}
//----------------------------------------------------------------------------
bool SimplePatch::Register (OutStream& target) const
{
    return SurfacePatch::Register(target);
}
//----------------------------------------------------------------------------
void SimplePatch::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    SurfacePatch::Save(target);

    target.Write(mAmplitude);

    WM5_END_DEBUG_STREAM_SAVE(SimplePatch, target);
}
//----------------------------------------------------------------------------
int SimplePatch::GetStreamingSize () const
{
    int size = SurfacePatch::GetStreamingSize();
    size += sizeof(mAmplitude);
    return size;
}
//----------------------------------------------------------------------------
