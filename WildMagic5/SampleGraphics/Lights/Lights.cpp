// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2013/07/14)

#include "Lights.h"

WM5_WINDOW_APPLICATION(Lights);

//----------------------------------------------------------------------------
Lights::Lights ()
    :
    WindowApplication3("SampleGraphics/Lights", 0, 0, 800, 600,
        Float4(0.0f, 0.25f, 0.75f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
}
//----------------------------------------------------------------------------
bool Lights::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 0.1f, 100.0f);
    APoint camPosition(16.0f, 0.0f, 8.0f);
    AVector camDVector = APoint::ORIGIN - camPosition;  // look at origin
    camDVector.Normalize();
    AVector camUVector(camDVector[2], 0.0f, -camDVector[0]);
    AVector camRVector(0.0f, 1.0f, 0.0f);
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
void Lights::OnTerminate ()
{
    mScene = 0;
    mWireState = 0;
    mPlane0 = 0;
    mSphere0 = 0;
    mPlane1 = 0;
    mSphere1 = 0;
    for (int i = 0; i < 3; ++i)
    {
        mLight[i][0] = 0;
        mLight[i][1] = 0;
        mInstance[i][0][0] = 0;
        mInstance[i][0][1] = 0;
        mInstance[i][1][0] = 0;
        mInstance[i][1][1] = 0;
    }

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void Lights::OnIdle ()
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
        mRenderer->Draw(8, 16, mTextColor, mCurrentCaption);
        DrawFrameRate(8, GetHeight()-8, mTextColor);
        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool Lights::OnKeyDown (unsigned char key, int x, int y)
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

    case 'd':
    case 'D':  // Switch to directional lights.
        mPlane0->SetEffectInstance(mInstance[0][0][0]);
        mSphere0->SetEffectInstance(mInstance[0][0][1]);
        mPlane1->SetEffectInstance(mInstance[0][1][0]);
        mSphere1->SetEffectInstance(mInstance[0][1][1]);
        mCurrentCaption = mCaption[0];
        mActiveType = 0;
        return true;

    case 'p':
    case 'P':  // Switch to point lights.
        mPlane0->SetEffectInstance(mInstance[1][0][0]);
        mSphere0->SetEffectInstance(mInstance[1][0][1]);
        mPlane1->SetEffectInstance(mInstance[1][1][0]);
        mSphere1->SetEffectInstance(mInstance[1][1][1]);
        mCurrentCaption = mCaption[1];
        mActiveType = 1;
        return true;

    case 's':
    case 'S':  // Switch to spot lights.
        mPlane0->SetEffectInstance(mInstance[2][0][0]);
        mSphere0->SetEffectInstance(mInstance[2][0][1]);
        mPlane1->SetEffectInstance(mInstance[2][1][0]);
        mSphere1->SetEffectInstance(mInstance[2][1][1]);
        mCurrentCaption = mCaption[2];
        mActiveType = 2;
        return true;

    case 'i':
        mLight[mActiveType][0]->Intensity -= 0.125f;
        mLight[mActiveType][1]->Intensity -= 0.125f;
        if (mLight[mActiveType][0]->Intensity < 0.0f)
        {
            mLight[mActiveType][0]->Intensity = 0.0f;
            mLight[mActiveType][1]->Intensity = 0.0f;
        }
        return true;

    case 'I':
        mLight[mActiveType][0]->Intensity += 0.125f;
        mLight[mActiveType][1]->Intensity += 0.125f;
        return true;

    case 'a':
        mLight[mActiveType][0]->Angle -= 0.1f;
        mLight[mActiveType][1]->Angle -= 0.1f;
        if (mLight[mActiveType][0]->Angle < 0.0f)
        {
            mLight[mActiveType][0]->Angle = 0.0f;
            mLight[mActiveType][1]->Angle = 0.0f;
        }
        mLight[mActiveType][0]->SetAngle(mLight[mActiveType][0]->Angle);
        mLight[mActiveType][1]->SetAngle(mLight[mActiveType][1]->Angle);
        return true;

    case 'A':
        mLight[mActiveType][0]->Angle += 0.1f;
        mLight[mActiveType][1]->Angle += 0.1f;
        if (mLight[mActiveType][0]->Angle > Mathf::HALF_PI)
        {
            mLight[mActiveType][0]->Angle = Mathf::HALF_PI;
            mLight[mActiveType][1]->Angle = Mathf::HALF_PI;
        }
        mLight[mActiveType][0]->SetAngle(mLight[mActiveType][0]->Angle);
        mLight[mActiveType][1]->SetAngle(mLight[mActiveType][1]->Angle);
        return true;

    case 'e':
        mLight[mActiveType][0]->Exponent *= 0.5f;
        mLight[mActiveType][1]->Exponent *= 0.5f;
        return true;

    case 'E':
        mLight[mActiveType][0]->Exponent *= 2.0f;
        mLight[mActiveType][1]->Exponent *= 2.0f;
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void Lights::CreateScene ()
{
    // Create the root of the scene.
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    // Create the lights.
    mLight[0][0] = new0 Light(Light::LT_DIRECTIONAL);
    mLight[0][0]->Ambient = Float4(0.75f, 0.75f, 0.75f, 1.0f);
    mLight[0][0]->Diffuse = Float4(1.0f, 1.0f, 1.0f, 1.0f);
    mLight[0][0]->Specular = Float4(1.0f, 1.0f, 1.0f, 1.0f);
    mLight[0][0]->SetDirection(AVector(-1.0f, -1.0f, -1.0f));
    mLight[0][1] = new0 Light(Light::LT_DIRECTIONAL);
    mLight[0][1]->Ambient = Float4(0.75f, 0.75f, 0.75f, 1.0f);
    mLight[0][1]->Diffuse = Float4(1.0f, 1.0f, 1.0f, 1.0f);
    mLight[0][1]->Specular = Float4(1.0f, 1.0f, 1.0f, 1.0f);
    mLight[0][1]->SetDirection(AVector(-1.0f, -1.0f, -1.0f));

    mLight[1][0] = new0 Light(Light::LT_POINT);
    mLight[1][0]->Ambient = Float4(0.1f, 0.1f, 0.1f, 1.0f);
    mLight[1][0]->Diffuse = Float4(1.0f, 1.0f, 1.0f, 1.0f);
    mLight[1][0]->Specular = Float4(1.0f, 1.0f, 1.0f, 1.0f);
    mLight[1][0]->Position = APoint(4.0f, 4.0f - 8.0f, 8.0f);
    mLight[1][1] = new0 Light(Light::LT_POINT);
    mLight[1][1]->Ambient = Float4(0.1f, 0.1f, 0.1f, 1.0f);
    mLight[1][1]->Diffuse = Float4(1.0f, 1.0f, 1.0f, 1.0f);
    mLight[1][1]->Specular = Float4(1.0f, 1.0f, 1.0f, 1.0f);
    mLight[1][1]->Position = APoint(4.0f, 4.0f + 8.0f, 8.0f);

    mLight[2][0] = new0 Light(Light::LT_SPOT);
    mLight[2][0]->Ambient = Float4(0.1f, 0.1f, 0.1f, 1.0f);
    mLight[2][0]->Diffuse = Float4(1.0f, 1.0f, 1.0f, 1.0f);
    mLight[2][0]->Specular = Float4(1.0f, 1.0f, 1.0f, 1.0f);
    mLight[2][0]->Position = APoint(4.0f, 4.0f - 8.0f, 8.0f);
    mLight[2][0]->SetDirection(AVector(-1.0f, -1.0f, -1.0f));
    mLight[2][0]->Exponent = 1.0f;
    mLight[2][0]->SetAngle(0.125f*Mathf::PI);
    mLight[2][1] = new0 Light(Light::LT_SPOT);
    mLight[2][1]->Ambient = Float4(0.1f, 0.1f, 0.1f, 1.0f);
    mLight[2][1]->Diffuse = Float4(1.0f, 1.0f, 1.0f, 1.0f);
    mLight[2][1]->Specular = Float4(1.0f, 1.0f, 1.0f, 1.0f);
    mLight[2][1]->Position = APoint(4.0f, 4.0f + 8.0f, 8.0f);
    mLight[2][1]->SetDirection(AVector(-1.0f, -1.0f, -1.0f));
    mLight[2][1]->Exponent = 1.0f;
    mLight[2][1]->SetAngle(0.125f*Mathf::PI);

    // The material shared by the planes (polished copper).
    Material* copper = new0 Material();
    copper->Emissive = Float4(0.0f, 0.0f, 0.0f, 1.0f);
    copper->Ambient = Float4(0.2295f, 0.08825f, 0.0275f, 1.0f);
    copper->Diffuse = Float4(0.5508f, 0.2118f, 0.066f, 1.0f);
    copper->Specular = Float4(0.580594f, 0.223257f, 0.0695701f, 51.2f);

    // The material shared by the spheres (polished gold).
    Material* gold = new0 Material();
    gold->Emissive = Float4(0.0f, 0.0f, 0.0f, 1.0f);
    gold->Ambient = Float4(0.24725f, 0.2245f, 0.0645f, 1.0f);
    gold->Diffuse = Float4(0.34615f, 0.3143f, 0.0903f, 1.0f);
    gold->Specular = Float4(0.797357f, 0.723991f, 0.208006f, 83.2f);

    // Create the effects.
    LightDirPerVerEffect* effectDV = new0 LightDirPerVerEffect();
    LightDirPerPixEffect* effectDP = new0 LightDirPerPixEffect();
    LightPntPerVerEffect* effectPV = new0 LightPntPerVerEffect();
    LightPntPerPixEffect* effectPP = new0 LightPntPerPixEffect();
    LightSptPerVerEffect* effectSV = new0 LightSptPerVerEffect();
    LightSptPerPixEffect* effectSP = new0 LightSptPerPixEffect();
    mInstance[0][0][0] = effectDV->CreateInstance(mLight[0][0], copper);
    mInstance[0][1][0] = effectDP->CreateInstance(mLight[0][1], copper);
    mInstance[1][0][0] = effectPV->CreateInstance(mLight[1][0], copper);
    mInstance[1][1][0] = effectPP->CreateInstance(mLight[1][1], copper);
    mInstance[2][0][0] = effectSV->CreateInstance(mLight[2][0], copper);
    mInstance[2][1][0] = effectSP->CreateInstance(mLight[2][1], copper);
    mInstance[0][0][1] = effectDV->CreateInstance(mLight[0][0], gold);
    mInstance[0][1][1] = effectDP->CreateInstance(mLight[0][1], gold);
    mInstance[1][0][1] = effectPV->CreateInstance(mLight[1][0], gold);
    mInstance[1][1][1] = effectPP->CreateInstance(mLight[1][1], gold);
    mInstance[2][0][1] = effectSV->CreateInstance(mLight[2][0], gold);
    mInstance[2][1][1] = effectSP->CreateInstance(mLight[2][1], gold);

    // Create the objects.  The normals are duplicated to texture
    // coordinates to avoid the AMD lighting problems due to use of
    // pre-OpenGL2.x extensions.
    VertexFormat* vformat = VertexFormat::Create(3,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_NORMAL, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT3, 1);

    StandardMesh sm(vformat);
    VertexBufferAccessor vba;

    mPlane0 = sm.Rectangle(128, 128, 8.0f, 8.0f);
    vba.ApplyTo(mPlane0);
    for (int i = 0; i < vba.GetNumVertices(); ++i)
    {
        vba.TCoord<Float3>(1, i) = vba.Normal<Float3>(i);
    }
    mPlane0->LocalTransform.SetTranslate(APoint(0.0f, -8.0f, 0.0f));
    mPlane0->SetEffectInstance(mInstance[0][0][0]);
    mScene->AttachChild(mPlane0);

    mSphere0 = sm.Sphere(64, 64, 2.0f);
    vba.ApplyTo(mSphere0);
    for (int i = 0; i < vba.GetNumVertices(); ++i)
    {
        vba.TCoord<Float3>(1, i) = vba.Normal<Float3>(i);
    }
    mSphere0->LocalTransform.SetTranslate(APoint(0.0f, -8.0f, 2.0f));
    mSphere0->SetEffectInstance(mInstance[0][0][1]);
    mScene->AttachChild(mSphere0);

    mPlane1 = sm.Rectangle(128, 128, 8.0f, 8.0f);
    vba.ApplyTo(mPlane1);
    for (int i = 0; i < vba.GetNumVertices(); ++i)
    {
        vba.TCoord<Float3>(1, i) = vba.Normal<Float3>(i);
    }
    mPlane1->LocalTransform.SetTranslate(APoint(0.0f, +8.0f, 0.0f));
    mPlane1->SetEffectInstance(mInstance[0][1][0]);
    mScene->AttachChild(mPlane1);

    mSphere1 = sm.Sphere(64, 64, 2.0f);
    vba.ApplyTo(mSphere1);
    for (int i = 0; i < vba.GetNumVertices(); ++i)
    {
        vba.TCoord<Float3>(1, i) = vba.Normal<Float3>(i);
    }
    mSphere1->LocalTransform.SetTranslate(APoint(0.0f, +8.0f, 2.0f));
    mSphere1->SetEffectInstance(mInstance[0][1][1]);
    mScene->AttachChild(mSphere1);

    mCaption[0] = "Directional Light (left per vertex, right per pixel)";
    mCaption[1] = "Point Light (left per vertex, right per pixel)";
    mCaption[2] = "Spot Light (left per vertex, right per pixel)";
    mCurrentCaption = mCaption[0];
    mActiveType = 0;
}
//----------------------------------------------------------------------------
