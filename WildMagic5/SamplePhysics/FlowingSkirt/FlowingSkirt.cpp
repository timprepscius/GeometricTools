// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "FlowingSkirt.h"

WM5_WINDOW_APPLICATION(FlowingSkirt);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
FlowingSkirt::FlowingSkirt ()
    :
    WindowApplication3("SamplePhysics/FlowingSkirt", 0, 0, 640, 480,
        Float4(0.75f, 0.75f, 0.75f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    mNumCtrl = 32;
    mDegree = 3;
    mATop = 1.0f;
    mBTop = 1.5f;
    mABot = 2.0f;
    mBBot = 3.0f;
    mSkirtTop = 0;
    mSkirtBot = 0;
    mFrequencies = new1<float>(mNumCtrl);
}
//----------------------------------------------------------------------------
FlowingSkirt::~FlowingSkirt ()
{
    delete1(mFrequencies);
}
//----------------------------------------------------------------------------
bool FlowingSkirt::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();

    // Center-and-fit for camera viewing.
    mScene->Update();
    mTrnNode->LocalTransform.SetTranslate(-mScene->WorldBound.GetCenter());
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 0.1f, 100.0f);
    AVector camDVector(0.0f, 0.0f, 1.0f);
    AVector camUVector(0.0f, 1.0f, 0.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    APoint camPosition = APoint::ORIGIN -
        2.5f*mScene->WorldBound.GetRadius()*camDVector;
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.005f, 0.01f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void FlowingSkirt::OnTerminate ()
{
    delete0(mSkirtTop);
    delete0(mSkirtBot);

    mScene = 0;
    mTrnNode = 0;
    mSkirt = 0;
    mWireState = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void FlowingSkirt::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    if (MoveObject())
    {
        mScene->Update();
    }

#ifndef SINGLE_STEP
    ModifyCurves();
#endif

    mCuller.ComputeVisibleSet(mScene);

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
bool FlowingSkirt::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication3::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
    case 'w':  // toggle wireframe
    case 'W':
        mWireState->Enabled = !mWireState->Enabled;
        return true;
#ifdef SINGLE_STEP
    case 'g':
    case 'G':
        ModifyCurves();
        return true;
#endif
    }

    return false;
}
//----------------------------------------------------------------------------
void FlowingSkirt::CreateScene ()
{
    mScene = new0 Node();
    mTrnNode = new0 Node();
    mScene->AttachChild(mTrnNode);
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    // The skirt top and bottom boundary curves are chosen to be periodic,
    // looped B-spline curves.  The top control points are generated on an
    // ellipse (x/a0)^2 + (z/b0)^2 = 1 with y = 4.  The bottom control points
    // are generated on an ellipse (x/a1)^2 + (z/b1)^2 = 1 with y = 0.

    // The vertex storage is used for the B-spline control points.  The
    // curve objects make a copy of the input points.  The vertex storage is
    // then used for the skirt mesh vertices themselves.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);
    int vstride = vformat->GetStride();

    int numVertices = 2*mNumCtrl;
    Vector3f* vertices = new1<Vector3f>(numVertices);
    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);

    int i, j;
    for (i = 0, j = mNumCtrl; i < mNumCtrl; ++i, ++j)
    {
        float ratio = ((float)i)/((float)mNumCtrl);
        float angle = Mathf::TWO_PI*ratio;
        float sn = Mathf::Sin(angle);
        float cs = Mathf::Cos(angle);
        float v = 1.0f - Mathf::FAbs(2.0f*ratio - 1.0f);

        // Set a vertex for the skirt top.
        vertices[i] = Vector3f(mATop*cs, 4.0f, mBTop*sn);
        vba.Position<Vector3f>(i) = vertices[i];
        vba.TCoord<Vector2f>(0, i) = Vector2f(1.0f, v);

        // Set a vertex for the skirt bottom.
        vertices[j] = Vector3f(mABot*cs, 0.0f, mBBot*sn);
        vba.Position<Vector3f>(j) = vertices[j];
        vba.TCoord<Float2>(0, j) = Float2(0.0f, v);

        // Frequency of sinusoidal motion for skirt bottom.
        mFrequencies[i] = 0.5f*(1.0f + Mathf::UnitRandom());
    }

    // The control points are copied by the curve objects.
    mSkirtTop = new0 BSplineCurve3f(mNumCtrl, vertices, mDegree, true, false);
    mSkirtBot = new0 BSplineCurve3f(mNumCtrl, &vertices[mNumCtrl], mDegree,
        true, false);

    delete1(vertices);

    // Generate the triangle connectivity (cylinder connectivity).
    int numTriangles = numVertices;
    int numIndices = 3*numTriangles;
    IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    int i0 = 0, i1 = 1, i2 = mNumCtrl, i3 = mNumCtrl + 1;
    for (i = 0; i1 < mNumCtrl; i0 = i1++, i2 = i3++)
    {
        indices[i++] = i0;
        indices[i++] = i1;
        indices[i++] = i3;
        indices[i++] = i0;
        indices[i++] = i3;
        indices[i++] = i2;
    }
    indices[i++] = mNumCtrl - 1;
    indices[i++] = 0;
    indices[i++] = mNumCtrl;
    indices[i++] = mNumCtrl - 1;
    indices[i++] = mNumCtrl;
    indices[i++] = 2*mNumCtrl - 1;

    mSkirt = new0 TriMesh(vformat, vbuffer, ibuffer);
    std::string path = Environment::GetPathR("Flower.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    VisualEffectInstance* instance = Texture2DEffect::CreateUniqueInstance(
        texture, Shader::SF_LINEAR, Shader::SC_CLAMP_EDGE,
        Shader::SC_CLAMP_EDGE);
    mSkirt->SetEffectInstance(instance);

    // Double-sided triangles.
    instance->GetEffect()->GetCullState(0, 0)->Enabled = false;

    // Compute the vertex values for the current B-spline curves.
    UpdateSkirt();

    mTrnNode->AttachChild(mSkirt);
}
//----------------------------------------------------------------------------
void FlowingSkirt::UpdateSkirt ()
{
    VertexBufferAccessor vba(mSkirt);
    for (int i = 0, j = mNumCtrl; i < mNumCtrl; ++i, ++j)
    {
        float t = ((float)i)/((float)mNumCtrl);
        vba.Position<Vector3f>(i) = mSkirtTop->GetPosition(t);
        vba.Position<Vector3f>(j) = mSkirtBot->GetPosition(t);
    }

    mSkirt->UpdateModelSpace(Visual::GU_MODEL_BOUND_ONLY);
    mSkirt->Update(0.0f);
    mRenderer->Update(mSkirt->GetVertexBuffer());
}
//----------------------------------------------------------------------------
void FlowingSkirt::ModifyCurves ()
{
    // Perturb the skirt bottom.
    float time = (float)GetTimeInSeconds();
    for (int i = 0; i < mNumCtrl; ++i)
    {
        float ratio = ((float)i)/((float)mNumCtrl);
        float angle = Mathf::TWO_PI*ratio;
        float sn = Mathf::Sin(angle);
        float cs = Mathf::Cos(angle);
        float amplitude = 1.0f + 0.25f*Mathf::Cos(mFrequencies[i]*time);
        mSkirtBot->SetControlPoint(i,
            Vector3f(amplitude*mABot*cs, 0.0f, amplitude*mBBot*sn));
    }

    UpdateSkirt();
}
//----------------------------------------------------------------------------
