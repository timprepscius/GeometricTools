// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "BSplineSurfaceFitter.h"

WM5_WINDOW_APPLICATION(BSplineSurfaceFitter);

//----------------------------------------------------------------------------
BSplineSurfaceFitter::BSplineSurfaceFitter ()
    :
    WindowApplication3("SampleMathematics/BSplineSurfaceFitter", 0, 0, 640,
        480, Float4(0.0f, 0.5f, 0.75f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
}
//----------------------------------------------------------------------------
bool BSplineSurfaceFitter::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 0.01f, 100.0f);
    APoint camPosition(0.0f, -9.0f, 1.5f);
    AVector camDVector(0.0f, 1.0f, 0.0f);
    AVector camUVector(0.0f, 0.0f, 1.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    CreateScene();

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.005f, 0.002f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void BSplineSurfaceFitter::OnTerminate ()
{
    mScene = 0;
    mWireState = 0;
    mCullState = 0;
    mHeightField = 0;
    mFittedField = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void BSplineSurfaceFitter::OnIdle ()
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
bool BSplineSurfaceFitter::OnKeyDown (unsigned char key, int x, int y)
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
    }

    return false;
}
//----------------------------------------------------------------------------
void BSplineSurfaceFitter::CreateScene ()
{
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);
    mCullState = new0 CullState();
    mCullState->Enabled = false;
    mRenderer->SetOverrideCullState(mCullState);

    // Begin with a flat 64x64 height field.
    const int numSamples = 64;
    const float extent = 8.0f;
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);
    mHeightField = StandardMesh(vformat).Rectangle(numSamples, numSamples,
        extent, extent);
    mScene->AttachChild(mHeightField);

    // Set the heights based on a precomputed height field.  Also create a
    // texture image to go with the height field.
    std::string path = Environment::GetPathR("HeightField.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    VisualEffectInstance* instance = Texture2DEffect::CreateUniqueInstance(
        texture, Shader::SF_LINEAR, Shader::SC_CLAMP_EDGE,
        Shader::SC_CLAMP_EDGE);
    mHeightField->SetEffectInstance(instance);
    unsigned char* data = (unsigned char*)texture->GetData(0);

    VertexBufferAccessor vba(mHeightField);
    Vector3f** samplePoints = new2<Vector3f>(numSamples, numSamples);
    int i;
    for (i = 0; i < vba.GetNumVertices(); ++i)
    {
        unsigned char value = *data;
        float height = 3.0f*((float)value)/255.0f +
            0.05f*Mathf::SymmetricRandom();

        *data++ = (unsigned char)Mathf::IntervalRandom(32.0f, 64.0f);
        *data++ = 3*(128 - value/2)/4;
        *data++ = 0;
        data++;

        vba.Position<Vector3f>(i).Z() = height;
        samplePoints[i % numSamples][i / numSamples] =
            vba.Position<Vector3f>(i);
    }

    // Compute a B-Spline surface with NxN control points, where N < 64.
    // This surface will be sampled to 64x64 and displayed together with the
    // original height field for comparison.
    const int numCtrlPoints = 32;
    const int degree = 3;
    BSplineSurfaceFitf fitter(degree, numCtrlPoints, numSamples, degree,
        numCtrlPoints, numSamples, samplePoints);
    delete2(samplePoints);

    vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT4, 0);
    mFittedField = StandardMesh(vformat).Rectangle(numSamples, numSamples,
        extent, extent);
    mScene->AttachChild(mFittedField);

    vba.ApplyTo(mFittedField);
    Float4 translucent(1.0f, 1.0f, 1.0f, 0.5f);
    for (i = 0; i < vba.GetNumVertices(); ++i)
    {
        float u = 0.5f*(vba.Position<Vector3f>(i).X()/extent + 1.0f);
        float v = 0.5f*(vba.Position<Vector3f>(i).Y()/extent + 1.0f);
        vba.Position<Vector3f>(i) = fitter.GetPosition(u, v);
        vba.Color<Float4>(0,i) = translucent;
    }

    instance = VertexColor4Effect::CreateUniqueInstance();
    mFittedField->SetEffectInstance(instance);
    instance->GetEffect()->GetAlphaState(0, 0)->BlendEnabled = true;
}
//----------------------------------------------------------------------------
