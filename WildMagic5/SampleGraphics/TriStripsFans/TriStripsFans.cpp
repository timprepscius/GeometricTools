// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "TriStripsFans.h"

WM5_WINDOW_APPLICATION(TriStripsFans);

//----------------------------------------------------------------------------
TriStripsFans::TriStripsFans ()
    :
    WindowApplication3("SampleGraphics/TriStripsFans", 0, 0, 512, 512,
        Float4(1.0f, 1.0f, 1.0f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    mActive = 0;
}
//----------------------------------------------------------------------------
bool TriStripsFans::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 0.1f, 100.0f);
    APoint camPosition(0.0f, 0.0f, 2.0f);
    AVector camDVector(0.0f, 0.0f, -1.0f);
    AVector camUVector(0.0f, 1.0f, 0.0f);
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
void TriStripsFans::OnTerminate ()
{
    mScene = 0;
    mWireState = 0;
    mStrip = 0;
    mStripMesh = 0;
    mFan = 0;
    mFanMesh = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void TriStripsFans::OnIdle ()
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
        if (mActive == mStrip)
        {
            if (mStrip->Culling == Spatial::CULL_ALWAYS)
            {
                mRenderer->Draw(128, GetHeight()-8, mTextColor,
                    "strip as mesh");
            }
            else
            {
                mRenderer->Draw(128, GetHeight()-8, mTextColor,
                    "strip");
            }
        }
        else
        {
            if (mFan->Culling == Spatial::CULL_ALWAYS)
            {
                mRenderer->Draw(128, GetHeight()-8, mTextColor,
                    "fan as mesh");
            }
            else
            {
                mRenderer->Draw(128, GetHeight()-8, mTextColor,
                    "fan");
            }
        }
        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool TriStripsFans::OnKeyDown (unsigned char key, int x, int y)
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

    case 'a':
    case 'A':
        if (mActive == mStrip)
        {
            mActive = mFan;
            mStrip->Culling = Spatial::CULL_ALWAYS;
            mStripMesh->Culling = Spatial::CULL_ALWAYS;
            mFan->Culling = Spatial::CULL_DYNAMIC;
            mFanMesh->Culling = Spatial::CULL_ALWAYS;
            mCuller.ComputeVisibleSet(mScene);
        }
        else
        {
            mActive = mStrip;
            mStrip->Culling = Spatial::CULL_DYNAMIC;
            mStripMesh->Culling = Spatial::CULL_ALWAYS;
            mFan->Culling = Spatial::CULL_ALWAYS;
            mFanMesh->Culling = Spatial::CULL_ALWAYS;
            mCuller.ComputeVisibleSet(mScene);
        }
        return true;

    case 'c':
    case 'C':
        if (mActive == mStrip)
        {
            if (mStrip->Culling == Spatial::CULL_ALWAYS)
            {
                mStrip->Culling = Spatial::CULL_DYNAMIC;
                mStripMesh->Culling = Spatial::CULL_ALWAYS;
            }
            else
            {
                mStrip->Culling = Spatial::CULL_ALWAYS;
                mStripMesh->Culling = Spatial::CULL_DYNAMIC;
            }
            mFan->Culling = Spatial::CULL_ALWAYS;
            mFanMesh->Culling = Spatial::CULL_ALWAYS;
            mCuller.ComputeVisibleSet(mScene);
        }
        else
        {
            if (mFan->Culling == Spatial::CULL_ALWAYS)
            {
                mFan->Culling = Spatial::CULL_DYNAMIC;
                mFanMesh->Culling = Spatial::CULL_ALWAYS;
            }
            else
            {
                mFan->Culling = Spatial::CULL_ALWAYS;
                mFanMesh->Culling = Spatial::CULL_DYNAMIC;
            }
            mStrip->Culling = Spatial::CULL_ALWAYS;
            mStripMesh->Culling = Spatial::CULL_ALWAYS;
            mCuller.ComputeVisibleSet(mScene);
        }
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void TriStripsFans::CreateScene ()
{
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    // Texture effect shared by all objects.
    Texture2DEffect* effect = new0 Texture2DEffect(Shader::SF_LINEAR);
    std::string path = Environment::GetPathR("Magician.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);

    // Create the vertices and texture coordinates for the cube tristrip
    // and the trimesh corresponding to it.
    int sideDivisions = 498;
    int sideNumVertices = 2*(2 + sideDivisions);
    int numVertices = 6*sideNumVertices;
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);

    const float scale = 2.0f/3.0f;
    int i, side;
    for (side = 0; side < 6; ++side)
    {
        // Create initial corner vertices for this side of the cube.
        Vector3f cornerVertex[4];
        Vector2f cornerUV[4];
        int axis = side >> 1;
        float b2 = ((side & 1) ? +1.0f : -1.0f);
        for (i = 0; i < 4; ++i)
        {
            float b0 = ((i & 1) ? +1.0f : -1.0f);
            float b1 = ((i & 2) ? +1.0f : -1.0f);

            switch (axis)
            {
            case 0:
                cornerVertex[i] = Vector3f(b2, -b0, -b1*b2)*scale;
                break;
            case 1:
                cornerVertex[i] = Vector3f(b1*b2, b2, b0)*scale;
                break;
            case 2:
                cornerVertex[i] = Vector3f(b1*b2, -b0, b2)*scale;
                break;
            }
            cornerUV[i][0] = (float)(1 - ((i >> 1) & 1));
            cornerUV[i][1] = (float)(1 - (i & 1));
        }

        // Interpolate between the corner vertices.
        int offset = side*sideNumVertices;
        for (i = 0; i < sideNumVertices; ++i)
        {
            int parity = (i & 1);
            float interpolation = (float)(i >> 1)/(float)(1+sideDivisions);
            int index = i + offset;
            assertion(index < numVertices, "Unexpected condition.\n");
            vba.Position<Vector3f>(index) = cornerVertex[parity] *
                (1.0f - interpolation) + cornerVertex[2 + parity] *
                interpolation;
            vba.TCoord<Vector2f>(0, index) = cornerUV[parity] *
                (1.0f-interpolation) + cornerUV[2 + parity]*interpolation;
        }
    }

    // Generate tristrip indices for the cube [T = 6*(sideNumVertices-2)].
    int numIndices = 6*sideNumVertices + 10;
    IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    for (side = 0; side < 6; ++side)
    {
        // Tristrip indices for the side.
        for (i = 0; i < sideNumVertices; ++i)
        {
            *indices++ = side*sideNumVertices + i;
        }

        // Join the next side by replication using the last vertex index from
        // the current side and the first vertex index from the next side.
        if (side < 5)
        {
            *indices++ = (side + 1)*sideNumVertices - 1;
            *indices++ = (side + 1)*sideNumVertices;
        }
    }

    mStrip = new0 TriStrip(vformat, vbuffer, ibuffer);
    mStrip->SetEffectInstance(effect->CreateInstance(texture));
    mStrip->Culling = Spatial::CULL_DYNAMIC;
    mScene->AttachChild(mStrip);

    // Construct the TriMesh object corresponding to the TriStrip object.
    // The vertex buffer from the TriStrip object should not be shared,
    // because the vertex normals are computed differently between TriMeshs
    // and TriStrips.
    VertexBuffer* meshVBuffer = new0 VertexBuffer(numVertices, vstride);
    VertexBufferAccessor meshVBA(vformat, meshVBuffer);
    for (i = 0; i < numVertices; ++i)
    {
        meshVBA.Position<Vector3f>(i) = vba.Position<Vector3f>(i);
        meshVBA.TCoord<Vector2f>(0, i) = vba.TCoord<Vector2f>(0, i);
    }

    // Convert tristrip indices to trimesh format.  First, count the number
    // of degenerate triangles.  Second, create the trimesh indices.
    int numTriangles = 0;
    int v0, v1, v2;
    for (i = 0; i < numIndices - 2; ++i)
    {
        if (mStrip->GetTriangle(i, v0, v1, v2))
        {
            ++numTriangles;
        }
    }
    assertion(numTriangles > 0, "Unexpected condition.\n");
    int meshNumIndices = 3*numTriangles;
    IndexBuffer* meshIBuffer = new0 IndexBuffer(meshNumIndices, sizeof(int));
    int* meshIndices = (int*)meshIBuffer->GetData();
    for (i = 0; i < numIndices - 2; i++)
    {
        if (mStrip->GetTriangle(i, v0, v1, v2))
        {
            *meshIndices++ = v0;
            *meshIndices++ = v1;
            *meshIndices++ = v2;
        }
    }

    mStripMesh = new0 TriMesh(vformat, meshVBuffer, meshIBuffer);
    mStripMesh->SetEffectInstance(effect->CreateInstance(texture));
    mStripMesh->Culling = Spatial::CULL_ALWAYS;
    mScene->AttachChild(mStripMesh);

    // Create the vertices and texture coordinates for the quarter-circle
    // tristrip and the trimesh corresponding to it.
    vbuffer = new0 VertexBuffer(numVertices + 1, vstride);
    vba.ApplyTo(vformat, vbuffer);

    vba.Position<Vector3f>(0) = Vector3f(-1.0f, -1.0f, 0.0f);
    vba.TCoord<Vector2f>(0, 0) = Vector2f(0.0f, 0.0f);
    for (i = 1; i <= numVertices; ++i)
    {
        float angle = Mathf::HALF_PI*(i - 1.0f)/(numVertices - 1.0f);
        float cs = Mathf::Cos(angle);
        float sn = Mathf::Sin(angle);
        vba.Position<Vector3f>(i) =
            Vector3f(2.0f*cs - 1.0f, 2.0f*sn - 1.0f, 0.0f);
        vba.TCoord<Vector2f>(0, i) = Vector2f(cs, sn);
    }

    mFan = new0 TriFan(vformat, vbuffer, 4);
    mFan->SetEffectInstance(effect->CreateInstance(texture));
    mFan->Culling = Spatial::CULL_ALWAYS;
    mScene->AttachChild(mFan);

    // Construct the TriMesh object corresponding to the TriStrip object.
    numTriangles = numVertices - 1;
    numIndices = 3*numTriangles;
    ibuffer = new0 IndexBuffer(numIndices, sizeof(int));
    indices = (int*)ibuffer->GetData();
    for (i = 0; i < numTriangles; ++i)
    {
        *indices++ = 0;
        *indices++ = i + 1;
        *indices++ = i + 2;
    }

    mFanMesh = new0 TriMesh(vformat, vbuffer, ibuffer);
    mFanMesh->SetEffectInstance(effect->CreateInstance(texture));
    mFanMesh->Culling = Spatial::CULL_ALWAYS;
    mScene->AttachChild(mFanMesh);

    mActive = mStrip;
}
//----------------------------------------------------------------------------
