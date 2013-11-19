// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "ParticleSystems.h"
#include "BloodCellController.h"

WM5_WINDOW_APPLICATION(ParticleSystems);

//----------------------------------------------------------------------------
ParticleSystems::ParticleSystems ()
    :
    WindowApplication3("SampleGraphics/ParticleSystems", 0, 0, 640, 480,
        Float4(0.5f, 0.0f, 1.0f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
}
//----------------------------------------------------------------------------
bool ParticleSystems::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 1000.0f);
    APoint camPosition(4.0f, 0.0f, 0.0f);
    AVector camDVector(-1.0f, 0.0f, 0.0f);
    AVector camUVector(0.0f, 0.0f, 1.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    CreateScene();

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.001f, 0.001f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void ParticleSystems::OnTerminate ()
{
    mScene = 0;
    mWireState = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void ParticleSystems::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    MoveObject();
    mScene->Update(GetTimeInSeconds());
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
bool ParticleSystems::OnKeyDown (unsigned char key, int x, int y)
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
void ParticleSystems::CreateScene ()
{
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);
    int vstride = vformat->GetStride();

    const int numParticles = 32;
    VertexBuffer* vbuffer = new0 VertexBuffer(4*numParticles, vstride);
    Float4* positionSizes = new1<Float4>(numParticles);
    for (int i = 0; i < numParticles; ++i)
    {
        positionSizes[i][0] = Mathf::SymmetricRandom();
        positionSizes[i][1] = Mathf::SymmetricRandom();
        positionSizes[i][2] = Mathf::SymmetricRandom();
        positionSizes[i][3] = 0.25f*Mathf::UnitRandom();
    }

    Particles* particles = new0 Particles(vformat, vbuffer, sizeof(int),
        positionSizes, 1.0f);

    particles->AttachController(new0 BloodCellController());
    mScene->AttachChild(particles);

    // Create an image with transparency.
    const int xsize = 32, ysize = 32;
    Texture2D* texture = new0 Texture2D(Texture::TF_A8R8G8B8, xsize,
        ysize, 1);
    unsigned char* data = (unsigned char*)texture->GetData(0);

    float factor = 1.0f/(xsize*xsize + ysize*ysize);
    for (int y = 0, i = 0; y < ysize; ++y)
    {
        for (int x = 0; x < xsize; ++x)
        {
            // The image is red.
            data[i++] = 0;
            data[i++] = 0;
            data[i++] = 255;

            // Semitransparent within a disk, dropping off to zero outside the
            // disk.
            int dx = 2*x - xsize;
            int dy = 2*y - ysize;
            float value = factor*(dx*dx + dy*dy);
            if (value < 0.125f)
            {
                value = Mathf::Cos(4.0f*Mathf::PI*value);
            }
            else
            {
                value = 0.0f;
            }
            data[i++] = (unsigned char)(255.0f*value);
        }
    }

    Texture2DEffect* effect = new0 Texture2DEffect(Shader::SF_LINEAR);
    effect->GetAlphaState(0, 0)->BlendEnabled = true;
    effect->GetDepthState(0, 0)->Enabled = false;
    particles->SetEffectInstance(effect->CreateInstance(texture));
}
//----------------------------------------------------------------------------
