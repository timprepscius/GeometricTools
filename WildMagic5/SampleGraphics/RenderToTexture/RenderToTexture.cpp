// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "RenderToTexture.h"

WM5_WINDOW_APPLICATION(RenderToTexture);

//----------------------------------------------------------------------------
RenderToTexture::RenderToTexture ()
    :
    WindowApplication3("SampleGraphics/RenderToTexture", 0, 0, 640, 480,
        Float4(0.5f, 0.5f, 0.5f, 1.0f)),
        mClearWhite(1.0f, 1.0f, 1.0f, 1.0f),
        mClearGray(0.5f, 0.5f, 0.5f, 1.0f),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
}
//----------------------------------------------------------------------------
bool RenderToTexture::OnInitialize ()
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
    AVector camDVector(0.0f, 1.0f, 0.0f);
    AVector camUVector(0.0f, 0.0f, 1.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    APoint camPosition = APoint::ORIGIN - 300.0f*camDVector;
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
void RenderToTexture::OnTerminate ()
{
    mScene = 0;
    mTrnNode = 0;
    mWireState = 0;
    mScreenCamera = 0;
    mRenderTarget = 0;
    mScreenPolygon = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void RenderToTexture::OnIdle ()
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
        // Draw the scene to a render target.
        mRenderer->Enable(mRenderTarget);
        mRenderer->SetClearColor(mClearWhite);
        mRenderer->ClearBuffers();
        mRenderer->Draw(mCuller.GetVisibleSet());
        mRenderer->Disable(mRenderTarget);

        // Draw the scene to the main window and also to a regular screen
        // polygon, placed in the lower-left corner of the main window.
        mRenderer->SetClearColor(mClearGray);
        mRenderer->ClearBuffers();
        mRenderer->Draw(mCuller.GetVisibleSet());
        mRenderer->SetCamera(mScreenCamera);
        mRenderer->Draw(mScreenPolygon);

        mRenderer->SetCamera(mCamera);
        DrawFrameRate(8, 16, mTextColor);
        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool RenderToTexture::OnKeyDown (unsigned char key, int x, int y)
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
void RenderToTexture::CreateScene ()
{
    // Create the root of the scene.
    mScene = new0 Node();
    mTrnNode = new0 Node();
    mScene->AttachChild(mTrnNode);
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    // Create a screen-space camera to use with the render target.
    mScreenCamera = ScreenTarget::CreateCamera();

    // Create a screen polygon to use with the render target.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    const int rtWidth = 256, rtHeight = 256;
    mScreenPolygon = ScreenTarget::CreateRectangle(vformat, rtWidth, rtHeight,
        0.0f, 0.2f, 0.0f, 0.2f, 0.0f);

    // Create the render target.
    //Texture::Format tformat = Texture::TF_A8B8G8R8;  // DX9 fails
    Texture::Format tformat = Texture::TF_A8R8G8B8;
    //Texture::Format tformat = Texture::TF_A16B16G16R16;
    //Texture::Format tformat = Texture::TF_A16B16G16R16F;
    //Texture::Format tformat = Texture::TF_A32B32G32R32F;
    mRenderTarget = new0 RenderTarget(1, tformat, rtWidth, rtHeight, false,
        false);

    // Attach the render target texture to the screen polygon mesh.
    mScreenPolygon->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(
        mRenderTarget->GetColorTexture(0), Shader::SF_LINEAR,
        Shader::SC_CLAMP_EDGE, Shader::SC_CLAMP_EDGE));

    // Load the face model and use multitexturing.
#ifdef WM5_LITTLE_ENDIAN
    std::string path = Environment::GetPathR("FacePN.wmof");
#else
    std::string path = Environment::GetPathR("FacePN.be.wmof");
#endif
    InStream inStream;
    inStream.Load(path);
    TriMeshPtr mesh = DynamicCast<TriMesh>(inStream.GetObjectAt(0));

    // Create texture coordinates for the face.  Based on knowledge of the
    // mesh, the (x,z) values of the model-space vertices may be mapped to
    // (s,t) in [0,1]^2.
    VertexBufferAccessor vba0(mesh);
    const int numVertices = vba0.GetNumVertices();
    float xmin = Mathf::MAX_REAL, xmax = -Mathf::MAX_REAL;
    float zmin = Mathf::MAX_REAL, zmax = -Mathf::MAX_REAL;
    int i;
    for (i = 1; i < numVertices; ++i)
    {
        Float3 position = vba0.Position<Float3>(i);
        float x = position[0];
        if (x < xmin)
        {
            xmin = x;
        }
        if (x > xmax)
        {
            xmax = x;
        }

        float z = position[2];
        if (z < zmin)
        {
            zmin = z;
        }
        if (z > zmax)
        {
            zmax = z;
        }
    }
    float invXRange = 1.0f/(xmax - xmin);
    float invZRange = 1.0f/(zmax - zmin);

    // Strip out the normal vectors, because there is no lighting in this
    // sample.  Add in two texture coordinate channels for a multiplicative
    // texture effect.
    vformat = VertexFormat::Create(3,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 1);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, vstride);
    VertexBufferAccessor vba1(vformat, vbuffer);
    for (i = 0; i < numVertices; ++i)
    {
        Float3 position = vba0.Position<Float3>(i);
        Float2 tcoord(
            (position[0] - xmin)*invXRange,
            (position[2] - zmin)*invZRange);

        vba1.Position<Float3>(i) = position;
        vba1.TCoord<Float2>(0, i) = tcoord;
        vba1.TCoord<Float2>(1, i) = tcoord;
    }
    mesh->SetVertexFormat(vformat);
    mesh->SetVertexBuffer(vbuffer);

    path = Environment::GetPathR("Leaf.wmtf");
    Texture2D* texture0 = Texture2D::LoadWMTF(path);
    path = Environment::GetPathR("Water.wmtf");
    Texture2D* texture1 = Texture2D::LoadWMTF(path);
    VisualEffectInstance* instance = Texture2AddEffect::CreateUniqueInstance(
        texture0, Shader::SF_LINEAR, Shader::SC_CLAMP_EDGE,
        Shader::SC_CLAMP_EDGE, texture1, Shader::SF_LINEAR,
        Shader::SC_CLAMP_EDGE, Shader::SC_CLAMP_EDGE);

    mesh->SetEffectInstance(instance);

    mTrnNode->AttachChild(mesh);
}
//----------------------------------------------------------------------------
