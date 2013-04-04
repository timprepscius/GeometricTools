// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.1.0 (2010/04/14)

#include "BlendedAnimations.h"

WM5_WINDOW_APPLICATION(BlendedAnimations);

//----------------------------------------------------------------------------
BlendedAnimations::BlendedAnimations ()
    :
    WindowApplication3("SampleGraphics/BlendedAnimations", 0, 0, 768, 768,
        Float4(0.5f, 0.0f, 1.0f, 1.0f)),
    mTextColor(1.0f, 1.0f, 1.0f, 1.0f),
    mManager(ThePath + "Data/", "Biped"),
    mAnimTime(0.0),
    mUpArrowPressed(false),
    mShiftPressed(false)
{
    // Set animation information.  The counts differ in debug and release
    // builds because of the differing frame rates of those builds.
#ifdef _DEBUG
    int idleWalkCount = 100;
    int walkCount = 10;
    int walkRunCount = 100;
    mAnimTimeDelta = 0.01;
#else
    int idleWalkCount = 1000;
    int walkCount = 100;
    int walkRunCount = 1000;
    mAnimTimeDelta = 0.001;
#endif

    // The idle head turning occurs too frequently (frequency = 1 in the
    // original model).  Reduce the turning by half.
    mManager.SetIdle(0.5, 0.0);

    // The walk and run cycles must be aligned properly for blending.  A
    // phase of 0.2 for the run cycle aligns the biped feet.
    mManager.SetRun(1.0, 0.2);

    // The initial state is 'idle'.
    mManager.Initialize(idleWalkCount, walkCount, walkRunCount);
}
//----------------------------------------------------------------------------
bool BlendedAnimations::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();

    // Center-and-fit for camera viewing.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 1000.0f);
    APoint camPosition(-60.0f, -60.0f, 90.0f);
    AVector camDVector(1.0f, 1.0f, -1.0f);
    camDVector.Normalize();
    AVector camUVector(0.5f, 0.5f, 1.0f);
    camUVector.Normalize();
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    // Initial update of objects.
    mScene->Update(mAnimTime);

    InitializeCameraMotion(0.01f, 0.01f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void BlendedAnimations::OnTerminate ()
{
    mScene = 0;
    mWireState = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void BlendedAnimations::OnIdle ()
{
    MeasureTime();

    if (MoveObject())
    {
        mScene->Update(mAnimTime);
    }

    Update();

    if (mRenderer->PreDraw())
    {
        mRenderer->ClearBuffers();
        mRenderer->Draw(mVisibleSet);

        mRenderer->Draw(8, 16, mTextColor,
            "Press UP-ARROW to transition from idle to walk.");

        mRenderer->Draw(8, 40, mTextColor,
            "Press SHIFT-UP-ARROW to transition from walk to run.");

        char message[128];
        sprintf(message, "time = %6.4lf", mAnimTime);
        mRenderer->Draw(128, GetHeight()-8, mTextColor, message);

        DrawFrameRate(8, GetHeight()-8, mTextColor);

        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool BlendedAnimations::OnKeyDown (unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'w':
    case 'W':
        mWireState->Enabled = !mWireState->Enabled;
        return true;
    }

    return WindowApplication3::OnKeyDown(key, x, y);
}
//----------------------------------------------------------------------------
bool BlendedAnimations::OnSpecialKeyDown (int key, int, int)
{
    if (key == KEY_UP_ARROW)
    {
        mUpArrowPressed = true;
    }
    else if (key == KEY_SHIFT)
    {
        mShiftPressed = true;
    }
    return true;
}
//----------------------------------------------------------------------------
bool BlendedAnimations::OnSpecialKeyUp (int key, int, int)
{
    if (key == KEY_UP_ARROW)
    {
        mUpArrowPressed = false;
    }
    else if (key == KEY_SHIFT)
    {
        mShiftPressed = false;
    }
    return true;
}
//----------------------------------------------------------------------------
void BlendedAnimations::CreateScene ()
{
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    mScene = new0 Node();
    mScene->AttachChild(mManager.GetRoot());

    // Create a floor to walk on.
    VertexFormat* vformat = VertexFormat::Create(3,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_NORMAL, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    mFloor = StandardMesh(vformat).Rectangle(2, 2, 1024.0f, 2048.0f);
    VertexBufferAccessor vba(mFloor);
    for (int i = 0; i < vba.GetNumVertices(); ++i)
    {
        Float2& tcoord = vba.TCoord<Float2>(0, i);
        tcoord[0] *= 64.0f;
        tcoord[1] *= 256.0f;
    }

    std::string textureName = Environment::GetPathR("Grating.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(textureName);
    texture->GenerateMipmaps();
    mFloor->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(texture,
        Shader::SF_LINEAR_LINEAR, Shader::SC_REPEAT, Shader::SC_REPEAT));

    mScene->AttachChild(mFloor);

    ComputeVisibleSet(mScene);
}
//----------------------------------------------------------------------------
void BlendedAnimations::ComputeVisibleSet (Spatial* object)
{
    TriMesh* mesh = DynamicCast<TriMesh>(object);
    if (mesh)
    {
        mVisibleSet.Insert(mesh);
        return;
    }

    Node* node = DynamicCast<Node>(object);
    if (node)
    {
        for (int i = 0; i < node->GetNumChildren(); ++i)
        {
            Spatial* child = node->GetChild(i);
            if (child)
            {
                ComputeVisibleSet(child);
            }
        }
    }
}
//----------------------------------------------------------------------------
void BlendedAnimations::Update ()
{
    // Animate the biped.
    mManager.Update(mUpArrowPressed, mShiftPressed);
    mScene->Update(mAnimTime);
    mAnimTime += mAnimTimeDelta;

    // Give the impression the floor is moving by translating the texture
    // coordinates.  For this demo, the texture coordinates are modified in
    // the vertex buffer.  Generally, you could write a vertex shader with
    // time and velocity as inputs, and then compute the new texture
    // coordinates in the shader.
#ifdef _DEBUG
    float adjust = mManager.GetSpeed()/16.0f;
#else
    float adjust = mManager.GetSpeed()/160.0f;
#endif
    VertexBufferAccessor vba(mFloor);
    for (int i = 0; i < vba.GetNumVertices(); ++i)
    {
        Float2& tcoord = vba.TCoord<Float2>(0, i);
        tcoord[1] -= adjust;
    }
    mRenderer->Update(mFloor->GetVertexBuffer());
}
//----------------------------------------------------------------------------
