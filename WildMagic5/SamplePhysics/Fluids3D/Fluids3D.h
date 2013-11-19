// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef FLUIDS3D_H
#define FLUIDS3D_H

#include "Wm5WindowApplication3.h"
#include "Smoke3D.h"
using namespace Wm5;

#define USE_PARTICLES

class Fluids3D : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    Fluids3D ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void PhysicsTick ();
    void GraphicsTick ();
    void CreateScene ();
    void UpdateVertexBuffer ();
    void UpdateIndexBuffer ();

    NodePtr mScene;
#ifdef USE_PARTICLES
    ParticlesPtr mCube;
#else
    TriMeshPtr mCube;
#endif
    int mNumIndices;
    int* mIndices;

    // Support for sorting the triangles by distance from the camera
    // position.
    class Triangle
    {
    public:
        float mNegSqrDistance;
        int mIndex0, mIndex1, mIndex2;

        bool operator< (const Triangle& triangle) const
        {
            return mNegSqrDistance < triangle.mNegSqrDistance;
        }
    };
    std::multiset<Triangle> mTriangles;

    Smoke3D<float>* mSmoke;
    Vector3f mColor[256];
    bool mSingleStep;
    bool mUseColor;
    bool mIndexBufferNeedsUpdate;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(Fluids3D);
WM5_REGISTER_TERMINATE(Fluids3D);

#endif
