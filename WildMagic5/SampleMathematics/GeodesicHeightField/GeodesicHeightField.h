// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef GEODESICHEIGHTFIELD_H
#define GEODESICHEIGHTFIELD_H

#include "Wm5WindowApplication3.h"
#include "Wm5BSplineGeodesic.h"
using namespace Wm5;

class GeodesicHeightField : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    GeodesicHeightField ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);
    virtual bool OnMouseClick (int button, int state, int x, int y,
        unsigned int modifiers);

protected:
    void CreateScene ();
    VisualEffectInstance* CreateEffectInstance ();
    void DrawLine (int x0, int y0, int x1, int y1, int width, char* data);
    void ComputeDistanceCurvature ();

    static void DrawCallback (int x, int y);
    static void RefineCallback ();

    NodePtr mScene;
    WireStatePtr mWireState;
    CullStatePtr mCullState;
    TriMeshPtr mMesh;
    Texture2DPtr mTexture;
    Culler mCuller;
    Picker mPicker;

    BSplineRectangled* mSurface;
    BSplineGeodesicd* mGeodesic;

    int mSelected;
    int mXIntr[2], mYIntr[2];
    GVectord mPoint[2];
    int mPQuantity;
    GVectord* mPath;
    double mDistance, mCurvature;

    Float4 mTextColor;

    // Hard-coded shader profile information.
    static int msDx9VRegisters[16];
    static int msOglVRegisters[16];
    static int* msVRegisters[Shader::MAX_PROFILES];
    static std::string msVPrograms[Shader::MAX_PROFILES];
    static int msAllPTextureUnits[1];
    static int* msPTextureUnits[Shader::MAX_PROFILES];
    static std::string msPPrograms[Shader::MAX_PROFILES];
};

WM5_REGISTER_INITIALIZE(GeodesicHeightField);
WM5_REGISTER_TERMINATE(GeodesicHeightField);

#endif
