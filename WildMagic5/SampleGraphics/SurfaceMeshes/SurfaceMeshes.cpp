// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "SurfaceMeshes.h"

WM5_WINDOW_APPLICATION(SurfaceMeshes);

//----------------------------------------------------------------------------
SurfaceMeshes::SurfaceMeshes ()
    :
    WindowApplication3("SampleGraphics/SurfaceMeshes", 0, 0, 640, 480,
        Float4(0.5f, 0.0f, 1.0f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
}
//----------------------------------------------------------------------------
bool SurfaceMeshes::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 100.0f);
    APoint camPosition(0.0f, 0.0f, 12.0f);
    AVector camDVector(0.0f, 0.0f, -1.0f);
    AVector camUVector(0.0f, 1.0f, 0.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    CreateScene();

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.01f, 0.001f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void SurfaceMeshes::OnTerminate ()
{
    mScene = 0;
    mWireState = 0;
    mSegment = 0;
    mCurve = 0;
    mPatch = 0;
    mSurface = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void SurfaceMeshes::OnIdle ()
{
    MeasureTime();

    if (MoveCamera())
    {
        mCuller.ComputeVisibleSet(mScene);
    }

    if (MoveObject())
    {
        mScene->Update();
        mCuller.ComputeVisibleSet(mScene);
    }

    if (mRenderer->PreDraw())
    {
        mRenderer->ClearBuffers();
        mRenderer->Draw(mCuller.GetVisibleSet());
        DrawFrameRate(8, GetHeight()-8, mTextColor);
        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool SurfaceMeshes::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication3::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
    case 'w':
    case 'W':
        mWireState->Enabled = !mWireState->Enabled;
        return true;
    case '+':
    case '=':
        if (mLevel < 5)
        {
            ++mLevel;
            mCurve->SetLevel(mLevel);
            mSurface->SetLevel(mLevel);
            mScene->Update();
            mCuller.ComputeVisibleSet(mScene);
        }
        return true;
    case '-':
    case '_':
        if (mLevel > 0)
        {
            --mLevel;
            mCurve->SetLevel(mLevel);
            mSurface->SetLevel(mLevel);
            mScene->Update();
            mCuller.ComputeVisibleSet(mScene);
        }
        return true;
    case 'l':
    case 'L':
        mCurve->Lock();
        mSurface->Lock();
        return true;
    case 'a':
        mPatch->SetAmplitude(mPatch->GetAmplitude() - 0.1f);
        mSurface->OnDynamicChange();
        mSurface->Update();
        mCuller.ComputeVisibleSet(mScene);
        return true;
    case 'A':
        mPatch->SetAmplitude(mPatch->GetAmplitude() + 0.1f);
        mSurface->OnDynamicChange();
        mSurface->Update();
        mCuller.ComputeVisibleSet(mScene);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void SurfaceMeshes::CreateScene ()
{
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);
    mCullState = new0 CullState();
    mCullState->Enabled = false;
    mRenderer->SetOverrideCullState(mCullState);

    CreateSimpleSegment();
    CreateSimplePatch();

    // Start with level zero on both segment and patch.
    mLevel = 0;
}
//----------------------------------------------------------------------------
void SurfaceMeshes::CreateSimpleSegment ()
{
    mSegment = new0 SimpleSegment();

    // Parameters for the initial vertex construction.
    const int numVertices = 4;
    float vParam[numVertices];
    vParam[0] = 0.0f;
    vParam[1] = 1.0f/3.0f;
    vParam[2] = 2.0f/3.0f;
    vParam[3] = 1.0f;

    // Initial vertex construction.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);
    vba.Position<Float3>(0) = mSegment->P(vParam[0]);
    vba.Position<Float3>(1) = mSegment->P(vParam[1]);
    vba.Position<Float3>(2) = mSegment->P(vParam[2]);
    vba.Position<Float3>(3) = mSegment->P(vParam[3]);
    vba.Color<Float3>(0, 0) = Float3(0.0f, 0.0f, 0.0f);
    vba.Color<Float3>(0, 1) = Float3(1.0f, 0.0f, 0.0f);
    vba.Color<Float3>(0, 2) = Float3(0.0f, 1.0f, 0.0f);
    vba.Color<Float3>(0, 3) = Float3(0.0f, 0.0f, 1.0f);

    // Each edge has its own parameters associated with its vertices.  A
    // vertex can have different parameters for the edges sharing it.
    const int numParams = 2*(numVertices - 1);
    float* params = new1<float>(numParams);
    params[0] = vParam[0];
    params[1] = vParam[1];
    params[2] = vParam[1];
    params[3] = vParam[2];
    params[4] = vParam[2];
    params[5] = vParam[3];
    FloatArray* fparams = new0 FloatArray(numParams, params);

    // The simple segment is used for all curve segments.
    const int numSegments = numVertices - 1;
    CurveSegmentPtr* segments = new1<CurveSegmentPtr>(numSegments);
    for (int i = 0; i < numSegments; ++i)
    {
        segments[i] = mSegment;
    }

    mCurve = new0 CurveMesh(vformat, vbuffer, segments, fparams, true);
    mCurve->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());
    mScene->AttachChild(mCurve);
}
//----------------------------------------------------------------------------
void SurfaceMeshes::CreateSimplePatch ()
{
    mPatch = new0 SimplePatch();

    // Parameters for the initial vertex construction.
    const int numVertices = 5;
    Float2 vParam[numVertices];
    vParam[0] = Float2(-1.0f, -1.0f);
    vParam[1] = Float2( 1.0f, -1.0f);
    vParam[2] = Float2( 1.0f,  1.0f);
    vParam[3] = Float2(-1.0f,  1.0f);
    vParam[4] = Float2( 2.0f,  0.0f);

    // Initial vertex construction.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);
    vba.Position<Float3>(0) = mPatch->P(vParam[0][0], vParam[0][1]);
    vba.Position<Float3>(1) = mPatch->P(vParam[1][0], vParam[1][1]);
    vba.Position<Float3>(2) = mPatch->P(vParam[2][0], vParam[2][1]);
    vba.Position<Float3>(3) = mPatch->P(vParam[3][0], vParam[3][1]);
    vba.Position<Float3>(4) = mPatch->P(vParam[4][0], vParam[4][1]);
    vba.TCoord<Float2>(0, 0) = Float2(0.0f, 0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(0.5f, 0.0f);
    vba.TCoord<Float2>(0, 2) = Float2(0.5f, 1.0f);
    vba.TCoord<Float2>(0, 3) = Float2(0.0f, 1.0f);
    vba.TCoord<Float2>(0, 4) = Float2(1.0f, 0.5f);

    // Initial index construction.
    const int numTriangles = 3, numIndices = 3*numTriangles;
    IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
    indices[3] = 0;  indices[4] = 2;  indices[5] = 3;
    indices[6] = 1;  indices[7] = 4;  indices[8] = 2;

    // Each triangle has its own parameters associated with its vertices.  A
    // vertex can have different parameters for the triangles sharing it.
    Float2* params = new1<Float2>(numIndices);
    int i;
    for (i = 0; i < numIndices; ++i)
    {
        params[i] = vParam[indices[i]];
    }
    Float2Array* fparams = new0 Float2Array(numIndices, params);

    // The simple patch is used for all triangles.
    SurfacePatchPtr* patches = new1<SurfacePatchPtr>(numTriangles);
    for (i = 0; i < numTriangles; ++i)
    {
        patches[i] = mPatch;
    }

    mSurface = new0 SurfaceMesh(vformat, vbuffer, ibuffer, fparams,
        patches, true);
    std::string path = Environment::GetPathR("Magician.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    mSurface->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(texture,
        Shader::SF_LINEAR, Shader::SC_CLAMP_EDGE, Shader::SC_CLAMP_EDGE));
    mScene->AttachChild(mSurface);
}
//----------------------------------------------------------------------------
