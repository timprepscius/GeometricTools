// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "VolumeTextures.h"
#include "VolumeTextureEffect.h"

WM5_WINDOW_APPLICATION(VolumeTextures);

//----------------------------------------------------------------------------
VolumeTextures::VolumeTextures ()
    :
    WindowApplication3("SampleGraphics/VolumeTextures", 0, 0, 640, 480,
        Float4(1.0f, 1.0f, 1.0f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    Environment::InsertDirectory(ThePath + "Shaders/");
}
//----------------------------------------------------------------------------
bool VolumeTextures::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 0.01f, 100.0f);
    APoint camPosition(0.0f, 0.0f, 4.0f);
    AVector camDVector(0.0f, 0.0f, -1.0f);
    AVector camUVector(0.0f, 1.0f, 0.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    CreateScene();

    // Initial update of objects.
    mScene->Update();

    // initial culling of scene
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.005f, 0.002f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void VolumeTextures::OnTerminate ()
{
    mScene = 0;
    mAlphaState = 0;
    mCullState = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void VolumeTextures::OnIdle ()
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
void VolumeTextures::CreateScene ()
{
    mScene = new0 Node();
    mAlphaState = new0 AlphaState();
    mAlphaState->BlendEnabled = true;
    mRenderer->SetOverrideAlphaState(mAlphaState);
    mCullState = new0 CullState();
    mCullState->Enabled = false;
    mRenderer->SetOverrideCullState(mCullState);

    // Create the grid of square meshes.
    const int numSlices = 64;
    const int numSamples = 32;

    // The vertex format that is shared by all square meshes.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    // The index buffer that is shared by all square meshes.
    int numIndices = 6*(numSamples-1)*(numSamples-1);
    IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    for (int i1 = 0; i1 < numSamples - 1; ++i1)
    {
        for (int i0 = 0; i0 < numSamples - 1; ++i0)
        {
            int v0 = i0 + numSamples * i1;
            int v1 = v0 + 1;
            int v2 = v1 + numSamples;
            int v3 = v0 + numSamples;
            *indices++ = v0;
            *indices++ = v1;
            *indices++ = v2;
            *indices++ = v0;
            *indices++ = v2;
            *indices++ = v3;
        }
    }

    // Create the volume texture.  Three Gaussian distributions are used for
    // the RGB color channels.  The alpha channel is constant.
    const int bound = 64;
    Texture3D* texture = new0 Texture3D(Texture::TF_A8R8G8B8, bound, bound,
        bound, 1);
    unsigned char* data = (unsigned char*)texture->GetData(0);
    const float mult = 1.0f/(bound - 1.0f);
    const float rParam = 0.01f;
    const float gParam = 0.01f;
    const float bParam = 0.01f;
    const float extreme = 8.0f;
    APoint rCenter( 0.5f*extreme,  0.0f,         0.0f);
    APoint gCenter(-0.5f*extreme, -0.5f*extreme, 0.0f);
    APoint bCenter(-0.5f*extreme, +0.5f*extreme, 0.0f);
    unsigned char commonAlpha = 12;
    APoint point;
    for (int z = 0; z < bound; ++z)
    {
        point[2] = -extreme + 2.0f*extreme*mult*z;
        for (int y = 0; y < bound; ++y)
        {
            point[1] = -extreme + 2.0f*extreme*mult*y;
            for (int x = 0; x < bound; ++x)
            {
                point[0] = -extreme + 2.0f*extreme*mult*x;

                AVector diff = point - rCenter;
                float sqrLength = diff.SquaredLength();
                float rGauss = 1.0f - rParam*sqrLength;
                if (rGauss < 0.0f)
                {
                    rGauss = 0.0f;
                }

                diff = point - gCenter;
                sqrLength = diff.SquaredLength();
                float gGauss = 1.0f - gParam*sqrLength;
                if (gGauss < 0.0f)
                {
                    gGauss = 0.0f;
                }

                diff = point - bCenter;
                sqrLength = diff.SquaredLength();
                float bGauss = 1.0f - bParam*sqrLength;
                if (bGauss < 0.0f)
                {
                    bGauss = 0.0f;
                }

                *data++ = (unsigned char)(255.0f*bGauss);
                *data++ = (unsigned char)(255.0f*gGauss);
                *data++ = (unsigned char)(255.0f*rGauss);
                *data++ = commonAlpha;
            }
        }
    }

    // The volume texture effect that is shared by all square meshes.
    std::string effectFile = Environment::GetPathR("VolumeTextures.wmfx");
    VolumeTextureEffect* effect = new0 VolumeTextureEffect(effectFile);
    VisualEffectInstance* instance = effect->CreateInstance(texture);

    // The grid of squares.
    const int numVertices = numSamples*numSamples;
    float inv = 1.0f/(numSamples - 1.0f);
    VertexBufferAccessor vba;
    for (int slice = 0; slice < numSlices; ++slice)
    {
        VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, vstride);
        vba.ApplyTo(vformat, vbuffer);

        float w = slice/(numSlices - 1.0f);
        float z = 2.0f*w - 1.0f;
        for (int i1 = 0, i = 0; i1 < numSamples; ++i1)
        {
            float v = i1*inv;
            float y = 2.0f*v - 1.0f;
            for (int i0 = 0; i0 < numSamples; ++i0, ++i)
            {
                float u = i0*inv;
                float x = 2.0f*u - 1.0f;
                vba.Position<Float3>(i) = Float3(x, y, z);
                vba.TCoord<Float3>(0, i) = Float3(u, v, w);
            }
        }

        TriMesh* mesh = new0 TriMesh(vformat, vbuffer, ibuffer);
        mesh->SetEffectInstance(instance);
        mScene->AttachChild(mesh);
    }
}
//----------------------------------------------------------------------------
