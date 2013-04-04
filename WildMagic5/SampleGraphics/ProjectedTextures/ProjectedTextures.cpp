// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "ProjectedTextures.h"
#include "ProjectedTextureEffect.h"

WM5_WINDOW_APPLICATION(ProjectedTextures);

//----------------------------------------------------------------------------
ProjectedTextures::ProjectedTextures ()
    :
    WindowApplication3("SampleGraphics/ProjectedTextures", 0, 0, 640, 480,
        Float4(1.0f, 1.0f, 1.0f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    Environment::InsertDirectory(ThePath + "Shaders/");
}
//----------------------------------------------------------------------------
bool ProjectedTextures::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();

    // Center-and-fit for camera viewing.
    mScene->Update();
    mTrnNode->LocalTransform.SetTranslate(-mScene->WorldBound.GetCenter());
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 1000.0f);
    AVector camDVector(0.0f,1.0f,0.0f);
    AVector camUVector(0.0f,0.0f,1.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    APoint camPosition = APoint::ORIGIN -
        2.0f*mScene->WorldBound.GetRadius()*camDVector;
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.1f, 0.001f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void ProjectedTextures::OnTerminate ()
{
    mScene = 0;
    mTrnNode = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void ProjectedTextures::OnIdle ()
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
void ProjectedTextures::CreateScene ()
{
    mScene = new0 Node();
    mTrnNode = new0 Node();
    mScene->AttachChild(mTrnNode);

    // Load the face model.
#ifdef WM5_LITTLE_ENDIAN
    std::string path = Environment::GetPathR("FacePN.wmof");
#else
    std::string path = Environment::GetPathR("FacePN.be.wmof");
#endif
    InStream inStream;
    inStream.Load(path);
    TriMesh* mesh = StaticCast<TriMesh>(inStream.GetObjectAt(0));

    // Create a camera to project the texture.
    Projector* projector = new0 Projector(Camera::PM_DEPTH_ZERO_TO_ONE);
    projector->SetFrustum(1.0f, 10.0f, -0.4125f, 0.4125f, -0.55f, 0.55f);
    AVector proDVector(0.0f, 1.0f, 0.0f);
    AVector proUVector(0.0f, 0.0f, 1.0f);
    AVector proRVector = proDVector.Cross(proUVector);
    APoint proPosition = APoint::ORIGIN - 303.0f*proDVector;
    projector->SetFrame(proPosition, proDVector, proUVector, proRVector);

    // Create a directional light for the face.
    Light* light = new0 Light(Light::LT_DIRECTIONAL);
    light->Ambient = Float4(0.25f, 0.25f, 0.25f, 1.0f);
    light->Diffuse = Float4(1.0f, 1.0f, 1.0f, 1.0f);
    light->Specular = Float4(0.0f, 0.0f, 0.0f, 1.0f);
    light->DVector = AVector::UNIT_Y;  // scene-camera direction

    // Create a material for the face.
    Material* material = new0 Material();
    material->Emissive = Float4(0.0f, 0.0f, 0.0f, 1.0f);
    material->Ambient = Float4(0.5f, 0.5f, 0.5f, 1.0f);
    material->Diffuse = Float4(0.99607f, 0.83920f, 0.67059f, 1.0f);
    material->Specular = Float4(0.8f, 0.8f, 0.8f, 0.0f);

    // Create the effect.
    std::string effectFile = Environment::GetPathR("ProjectedTexture.wmfx");
    ProjectedTextureEffect* effect = new0 ProjectedTextureEffect(effectFile);

    std::string projectedName = Environment::GetPathR("Magician.wmtf");
    Texture2D* projectedTexture = Texture2D::LoadWMTF(projectedName);
    mesh->SetEffectInstance(effect->CreateInstance(projector, light,
        material, projectedTexture));

    mTrnNode->AttachChild(mesh);
}
//----------------------------------------------------------------------------
