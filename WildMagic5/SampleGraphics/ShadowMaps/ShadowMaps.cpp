// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "ShadowMaps.h"

WM5_WINDOW_APPLICATION(ShadowMaps);

//----------------------------------------------------------------------------
ShadowMaps::ShadowMaps ()
    :
    WindowApplication3("SampleGraphics/ShadowMaps", 0, 0, 640, 480,
        Float4(0.75f, 0.75f, 0.75f, 1.0f)),
        mShadowClear(1.0f, 1.0f, 1.0f, 1.0f),
        mUnlitClear(1.0f, 1.0f, 1.0f, 1.0f),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    Environment::InsertDirectory(ThePath + "Shaders/");
}
//----------------------------------------------------------------------------
bool ShadowMaps::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, 1.0f, 0.1f, 100.0f);
    APoint camPosition(8.0f, 0.0f, 4.0f);
    AVector camDVector = APoint::ORIGIN - camPosition;  // look at origin
    camDVector.Normalize();
    AVector camUVector(camDVector[2], 0, -camDVector[0]);
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
void ShadowMaps::OnTerminate ()
{
    mScene = 0;

    mScreenCamera = 0;
    mScreenPolygon = 0;
    mShadowTarget = 0;
    mUnlitTarget = 0;
    mHBlurTarget = 0;
    mVBlurTarget = 0;

    mPlaneSceneInstance = 0;
    mSphereSceneInstance = 0;
    mShadowEffect = 0;
    mUnlitEffect = 0;
    mHBlurInstance = 0;
    mVBlurInstance = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
//#define DUMP_RENDER_TARGETS
//----------------------------------------------------------------------------
void ShadowMaps::OnIdle ()
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
        // Draw the scene from the light's perspective, writing the depths
        // from the light into the render target.
        mRenderer->Enable(mShadowTarget);
        mRenderer->SetClearColor(mShadowClear);
        mRenderer->ClearBuffers();
        mRenderer->Draw(mCuller.GetVisibleSet(), mShadowEffect);
        mRenderer->Disable(mShadowTarget);
#ifdef DUMP_RENDER_TARGETS
        Texture2D* texture = mRenderer->Read(mShadowTarget);
        texture->SaveWMTF("ShadowTarget.wmtf");
        delete0(texture);
#endif

        // Draw the scene from the camera's perspective, using projected
        // texturing of the shadow map and determining which pixels are lit
        // and which are shadowed.
        mRenderer->ClearBuffers();
        mRenderer->Enable(mUnlitTarget);
        mRenderer->SetClearColor(mUnlitClear);
        mRenderer->ClearBuffers();
        mRenderer->Draw(mCuller.GetVisibleSet(), mUnlitEffect);
        mRenderer->Disable(mUnlitTarget);
#ifdef DUMP_RENDER_TARGETS
        texture = mRenderer->Read(mUnlitTarget);
        texture->SaveWMTF("UnlitTarget.wmtf");
        delete0(texture);
#endif

        // Do screen space drawing.
        mRenderer->SetCamera(mScreenCamera);

        // Horizontally blur the unlit render target.
        mScreenPolygon->SetEffectInstance(mHBlurInstance);
        mRenderer->Enable(mHBlurTarget);
        mRenderer->Draw(mScreenPolygon);
        mRenderer->Disable(mHBlurTarget);
#ifdef DUMP_RENDER_TARGETS
        texture = mRenderer->Read(mHBlurTarget);
        texture->SaveWMTF("HBlurTarget.wmtf");
        delete0(texture);
#endif

        // Vertically blur the horizontal blur render target.
        mScreenPolygon->SetEffectInstance(mVBlurInstance);
        mRenderer->Enable(mVBlurTarget);
        mRenderer->Draw(mScreenPolygon);
        mRenderer->Disable(mVBlurTarget);
#ifdef DUMP_RENDER_TARGETS
        texture = mRenderer->Read(mVBlurTarget);
        texture->SaveWMTF("VBlurTarget.wmtf");
        delete0(texture);
#endif

        // Draw the scene using regular textures, combining the shadow
        // information with the scene.
        mRenderer->SetCamera(mCamera);
        mRenderer->SetClearColor(mClearColor);
        mRenderer->ClearBuffers();
        mRenderer->Draw(mCuller.GetVisibleSet());
        DrawFrameRate(8, GetHeight()-8, mTextColor);
        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
void ShadowMaps::CreateScene ()
{
    CreateScreenSpaceObjects();
    CreateShaders();

    // Create a scene graph containing a sphere and a plane.  The sphere will
    // cast a shadow on the plane.
    mScene = new0 Node();

    VertexFormat* vformat = VertexFormat::Create(3,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_NORMAL, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    StandardMesh sm(vformat);
    TriMesh* plane = sm.Rectangle(128, 128, 16.0f, 16.0f);
    plane->SetEffectInstance(mPlaneSceneInstance);
    mScene->AttachChild(plane);

    TriMesh* sphere = sm.Sphere(64, 64, 1.0f);
    sphere->LocalTransform.SetTranslate(APoint(0.0f, 0.0f, 1.0f));
    sphere->SetEffectInstance(mSphereSceneInstance);
    mScene->AttachChild(sphere);
}
//----------------------------------------------------------------------------
void ShadowMaps::CreateScreenSpaceObjects ()
{
    // Create a screen-space camera to use with the render target.
    mScreenCamera = ScreenTarget::CreateCamera();

    // Create a screen polygon to use with the render target.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    mScreenTargetSize = 512;
    mScreenPolygon = ScreenTarget::CreateRectangle(vformat, mScreenTargetSize,
        mScreenTargetSize, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

    // Create a render target for the shadow effect.
    mShadowTarget = new0 RenderTarget(1, Texture::TF_A32B32G32R32F,
        mScreenTargetSize, mScreenTargetSize, false, true);

    // Create a render target for the unlit scene.
    mUnlitTarget = new0 RenderTarget(1, Texture::TF_A32B32G32R32F, GetWidth(),
        GetHeight(), false, true);

    // Create a render target for the horizontally blurred scene.
    mHBlurTarget = new0 RenderTarget(1, Texture::TF_A8R8G8B8, GetWidth(),
        GetHeight(), false, false);

    // Create a render target for the vertically blurred scene.
    mVBlurTarget = new0 RenderTarget(1, Texture::TF_A8R8G8B8, GetWidth(),
        GetHeight(), false, false);

    mRenderer->Bind(mShadowTarget);
    mRenderer->Bind(mUnlitTarget);
    mRenderer->Bind(mHBlurTarget);
    mRenderer->Bind(mVBlurTarget);
}
//----------------------------------------------------------------------------
void ShadowMaps::CreateShaders ()
{
    // Create the shader constants.  Some of these are shared.

    // Create the light projector.
    Projector* projector = new0 Projector(Camera::PM_DEPTH_ZERO_TO_ONE);
    projector->SetFrustum(60.0f, 1.0f, 0.1f, 100.0f);
    APoint projPosition(4.0f, 4.0f, 4.0f);
    AVector projDVector(-1.0f, -1.0f, -1.0f);
    projDVector.Normalize();
    AVector projUVector(-1.0f, -1.0f, 2.0f);
    projUVector.Normalize();
    AVector projRVector = projDVector.Cross(projUVector);
    projector->SetFrame(projPosition, projDVector, projUVector, projRVector);

    // For SMSceneEffect and SMUnlitEffect.
    ProjectorMatrixConstant* lightPVMatrix =
        new0 ProjectorMatrixConstant(projector, false, 0);

    ShaderFloat* lightBSMatrixUnlit = new0 ShaderFloat(4);
    ShaderFloat* lightBSMatrixScene = new0 ShaderFloat(4);
    ShaderFloat* screenBSMatrix = new0 ShaderFloat(4);
    const float* src;
    if (VertexShader::GetProfile() == VertexShader::VP_ARBVP1)
    {
        src = (const float*)Projector::BiasScaleMatrix[1];
        memcpy(lightBSMatrixUnlit->GetData(), src, 16*sizeof(float));
        memcpy(lightBSMatrixScene->GetData(), src, 16*sizeof(float));
        memcpy(screenBSMatrix->GetData(), src, 16*sizeof(float));
    }
    else
    {
        src = (const float*)Projector::BiasScaleMatrix[0];
        memcpy(lightBSMatrixUnlit->GetData(), src, 16*sizeof(float));
        memcpy(screenBSMatrix->GetData(), src, 16*sizeof(float));
        src = (const float*)Projector::BiasScaleMatrix[1];
        memcpy(lightBSMatrixScene->GetData(), src, 16*sizeof(float));
    }

    // For SMSceneEffect.
    ProjectorWorldPositionConstant* lightWorldPosition =
        new0 ProjectorWorldPositionConstant(projector);

    ShaderFloat* lightColor = new0 ShaderFloat(1);
    (*lightColor)[0] = 1.0f;
    (*lightColor)[1] = 1.0f;
    (*lightColor)[2] = 1.0f;
    (*lightColor)[3] = 1.0f;

    // For SMUnlitEffect.
    ShaderFloat* depthBiasConstant = new0 ShaderFloat(1);
    (*depthBiasConstant)[0] = 0.1f;
    ShaderFloat* texelSizeConstant = new0 ShaderFloat(1);
    (*texelSizeConstant)[0] = 1.0f/(float)mScreenTargetSize;
    (*texelSizeConstant)[1] = 1.0f/(float)mScreenTargetSize;

    // For SMBlurEffect.
    const int numRegisters = 11;
    ShaderFloat* weights = new0 ShaderFloat(numRegisters);
    ShaderFloat* hOffsets = new0 ShaderFloat(numRegisters);
    ShaderFloat* vOffsets = new0 ShaderFloat(numRegisters);

    // Compute the weights.  They must sum to 1.
    Float4* weightsData = (Float4*)weights->GetData();
    const float stdDev = 1.0f;
    const float invTwoVariance = 1.0f/(2.0f*stdDev*stdDev);
    float totalWeight = 0.0f;
    int i, j;
    for (i = 0, j = -numRegisters/2; i < numRegisters; ++i, ++j)
    {
        float weight = Mathf::Exp(-j*j*invTwoVariance);
        weightsData[i] = Float4(weight, weight, weight, 0.0f);
        totalWeight += weight;
    }
    float invTotalWeight = 1.0f/totalWeight;
    for (i = 0; i < numRegisters; ++i)
    {
        weightsData[i][0] *= invTotalWeight;
        weightsData[i][1] *= invTotalWeight;
        weightsData[i][2] *= invTotalWeight;
    }

    // Compute the horizontal and vertical offsets.
    Float4* hOffsetsData = (Float4*)hOffsets->GetData();
    Float4* vOffsetsData = (Float4*)vOffsets->GetData();
    float uDelta = 1.0f/(float)GetWidth();
    float vDelta = 1.0f/(float)GetHeight();
    for (i = 0, j = -numRegisters/2; i < numRegisters; ++i, ++j)
    {
        hOffsetsData[i] = Float4(j*uDelta, 0.0f, 0.0f, 0.0f);
        vOffsetsData[i] = Float4(0.0f, j*vDelta, 0.0f, 0.0f);
    }

    // Create the scene effect.
    std::string effectFile = Environment::GetPathR("SMScene.wmfx");
    SMSceneEffect* sceneEffect = new0 SMSceneEffect(effectFile);

    std::string stoneName = Environment::GetPathR("Stone.wmtf");
    Texture2D* stoneTexture = Texture2D::LoadWMTF(stoneName);
    std::string ballName = Environment::GetPathR("BallTexture.wmtf");
    Texture2D* ballTexture = Texture2D::LoadWMTF(ballName);
    std::string projectedName = Environment::GetPathR("Magician.wmtf");
    Texture2D* projectedTexture = Texture2D::LoadWMTF(projectedName);

    mPlaneSceneInstance = sceneEffect->CreateInstance(lightWorldPosition,
        lightPVMatrix, lightBSMatrixScene, screenBSMatrix, lightColor,
        stoneTexture, mVBlurTarget->GetColorTexture(0), projectedTexture);

    mSphereSceneInstance = sceneEffect->CreateInstance(lightWorldPosition,
        lightPVMatrix, lightBSMatrixScene, screenBSMatrix, lightColor,
        ballTexture, mVBlurTarget->GetColorTexture(0), projectedTexture);

    // Create the shadow effect.
    effectFile = Environment::GetPathR("SMShadow.wmfx");
    mShadowEffect = new0 SMShadowEffect(effectFile, lightPVMatrix);

    // Create the unlit effect.
    effectFile = Environment::GetPathR("SMUnlit.wmfx");
    mUnlitEffect = new0 SMUnlitEffect(effectFile, lightPVMatrix,
        lightBSMatrixUnlit, depthBiasConstant, texelSizeConstant,
        mShadowTarget->GetColorTexture(0));

    // Create the blur effect and instantiate for horizontal and vertical
    // blurring.
    effectFile = Environment::GetPathR("SMBlur.wmfx");
    SMBlurEffect* blurEffect = new0 SMBlurEffect(effectFile);

    mHBlurInstance = blurEffect->CreateInstance(weights, hOffsets,
        mUnlitTarget->GetColorTexture(0));

    mVBlurInstance = blurEffect->CreateInstance(weights, vOffsets,
        mHBlurTarget->GetColorTexture(0));
}
//----------------------------------------------------------------------------
