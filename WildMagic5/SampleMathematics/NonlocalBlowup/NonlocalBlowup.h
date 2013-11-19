// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#ifndef NONLOCALBLOWUP_H
#define NONLOCALBLOWUP_H

#include "Wm5WindowApplication3.h"
#include "NonlocalSolver2.h"
#include "Image2.h"
using namespace Wm5;

// DX9 profile vs_3_0 and OpenGL profile vp40 support vertex textures.  You
// cannot run this sample without hardware supporting these profiles.

class NonlocalBlowup : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    NonlocalBlowup ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    // The grid is DIMENSION-by-DIMENSION samples.
    enum
    { 
        DIMENSION = 256,
        NUMSAMPLES = DIMENSION*DIMENSION
    };

    // Build a square domain for the PDE.
    void BuildSquareDomain ();

    // Build a nonconvex polygonal domain for the PDE.
    void BuildPolygonDomain ();

    // Create a height texture that corresponds to the initial data.  This
    // is used as a displacement map in a vertex shader to display the graph
    // of temperature.
    void CreateInitialHeight ();

    // u(x,y,0) = (1-x^2)*(1-y^2)
    void BuildInitialData0 ();

    // u(x,y,0) = (1-x^2)*(1-y^2)*exp(-r^2)/4,
    //   r = (x-0.5)/0.01
    void BuildInitialDataGaussX ();

    // u(x,y,0) = (1-x^2)*(1-y^2)*exp(-(r^2 + s^2))/4,
    //   r = (x-0.5)/0.01, s = (y-0.5)/0.01
    void BuildInitialDataGaussXY ();

    // u(x,y,0) = (1-x^2)*(1-y^2)*[e00 + e01 + e10 + e11]/4
    //   e00 = exp(-(r0^2 + s0^2)),
    //   e10 = exp(-(r0^2 + s1^2)),
    //   e01 = exp(-(r1^2 + s0^2)),
    //   e11 = exp(-(r1^2 + s1^2)),
    //   r0 = (x-0.5)/0.01, r1 = (x+0.5)/0.01,
    //   s0 = (y-0.5)/0.01, s1 = (y+0.5)/0.01
    void BuildInitialDataGaussFour ();

    // u(x,y,0) = (1-x^2)*(1-y^2)*exp(-(r^2 + s^2))/4,
    //   r = (x-0.5)/0.01, s = (y-0.75)/0.01
    void BuildInitialDataGaussXYOff ();

    // Choose one of these to illustrate blowup.
    void SquareSymmetric0p01 (float dt, float dx, float dy);
    void SquareSymmetric0p50 (float dt, float dx, float dy);
    void SquareSymmetric0p99 (float dt, float dx, float dy);
    void SquareGaussX0p50 (float dt, float dx, float dy);
    void SquareGaussXY0p50 (float dt, float dx, float dy);
    void SquareGaussFour0p50 (float dt, float dx, float dy);
    void NonconvexDomain0p50 (float dt, float dx, float dy);

    // Execute a single step of the solver.
    void ExecuteSolver ();

    // Read the back buffer and save to disk to make movies.
    void GetSnapshot ();

    NodePtr mScene;
    WireStatePtr mWireState;
    Culler mCuller;
    Float4 mTextColor;

    Image2<float> mInitialData;
    Image2<unsigned char> mDomain;
    Texture2D* mHeightTexture;
    Texture2D* mDomainTexture;
    NonlocalSolver2* mSolver;
    int mIteration;
    bool mIsFinite;

    std::string mPrefix;
    bool mTakeSnapshot;
};

WM5_REGISTER_INITIALIZE(NonlocalBlowup);
WM5_REGISTER_TERMINATE(NonlocalBlowup);

#endif
