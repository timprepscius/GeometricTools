// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "GlossMaps.h"
#include "GlossMapEffect.h"

WM5_WINDOW_APPLICATION(GlossMaps);

//----------------------------------------------------------------------------
GlossMaps::GlossMaps ()
    :
    WindowApplication3("SampleGraphics/GlossMaps", 0, 0, 640, 480,
        Float4(0.85f, 0.85f, 0.85f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    Environment::InsertDirectory(ThePath + "Shaders/");
}
//----------------------------------------------------------------------------
bool GlossMaps::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();

    // Center-and-fit for camera viewing.
    mScene->Update();
    mTrnNode->LocalTransform.SetTranslate(-mScene->WorldBound.GetCenter());
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 100.0f);
    AVector camDVector(0.0f,0.0f,1.0f);
    AVector camUVector(0.0f,1.0f,0.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    APoint camPosition = APoint::ORIGIN -
        2.0f*mScene->WorldBound.GetRadius()*camDVector;
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

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
void GlossMaps::OnTerminate ()
{
    mScene = 0;
    mTrnNode = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void GlossMaps::OnIdle ()
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
void GlossMaps::CreateScene ()
{
    mScene = new0 Node();
    mTrnNode = new0 Node();
    mScene->AttachChild(mTrnNode);

    // Create vertex and index buffers to be shared by two meshes.
    VertexFormat* vformat = VertexFormat::Create(3,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_NORMAL, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(4, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);
    Float3 yVector(0.0f, 1.0f, 0.0f);
    vba.Position<Float3>(0) = Float3(-0.5f, 0.0f, -0.5f);
    vba.Position<Float3>(1) = Float3(-0.5f, 0.0f,  0.5f);
    vba.Position<Float3>(2) = Float3( 0.5f, 0.0f,  0.5f);
    vba.Position<Float3>(3) = Float3( 0.5f, 0.0f, -0.5f);
    vba.Normal<Float3>(0) = yVector;
    vba.Normal<Float3>(1) = yVector;
    vba.Normal<Float3>(2) = yVector;
    vba.Normal<Float3>(3) = yVector;
    vba.TCoord<Float2>(0, 0) = Float2(1.0f, 0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(1.0f, 1.0f);
    vba.TCoord<Float2>(0, 2) = Float2(0.0f, 1.0f);
    vba.TCoord<Float2>(0, 3) = Float2(0.0f, 0.0f);

    IndexBuffer* ibuffer = new0 IndexBuffer(6, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 3;
    indices[3] = 3;  indices[4] = 1;  indices[5] = 2;

    // The light and material are used by both the gloss and non-gloss
    // objects.
    Light* light = new0 Light(Light::LT_DIRECTIONAL);
    light->Ambient = Float4(0.1f, 0.1f, 0.1f, 1.0f);
    light->Diffuse = Float4(0.6f, 0.6f, 0.6f, 1.0f);
    light->Specular = Float4(1.0f, 1.0f, 1.0f, 1.0f);
    light->DVector = AVector(0.0f, -1.0f, 0.0f);

    Material* material = new0 Material();
    material->Ambient = Float4(0.2f, 0.2f, 0.2f, 1.0f);
    material->Diffuse = Float4(0.7f, 0.7f, 0.7f, 1.0f);
    material->Specular = Float4(1.0f, 1.0f, 1.0f, 25.0f);

    // Create a non-gloss-mapped square.
    TriMesh* squareNoGloss = new0 TriMesh(vformat, vbuffer, ibuffer);
    squareNoGloss->LocalTransform.SetRotate(HMatrix(AVector::UNIT_X,
        -0.25f*Mathf::PI));
    squareNoGloss->LocalTransform.SetTranslate(APoint(1.0f, -1.0f, 0.0f));
    squareNoGloss->SetEffectInstance(
        LightDirPerVerEffect::CreateUniqueInstance(light, material));
    mTrnNode->AttachChild(squareNoGloss);

    // Create a gloss-mapped square.
    TriMesh* squareGloss = new0 TriMesh(vformat, vbuffer, ibuffer);
    squareGloss->LocalTransform.SetRotate(HMatrix(AVector::UNIT_X,
        -0.25f*Mathf::PI));
    squareGloss->LocalTransform.SetTranslate(APoint(-1.0f, -1.0f, 0.0f));
    mTrnNode->AttachChild(squareGloss);

    std::string effectFile = Environment::GetPathR("GlossMap.wmfx");
    GlossMapEffect* effect = new0 GlossMapEffect(effectFile);

    std::string baseName = Environment::GetPathR("Magic.wmtf");
    Texture2D* baseTexture = Texture2D::LoadWMTF(baseName);
    squareGloss->SetEffectInstance(effect->CreateInstance(baseTexture,
        light, material));
}
//----------------------------------------------------------------------------
