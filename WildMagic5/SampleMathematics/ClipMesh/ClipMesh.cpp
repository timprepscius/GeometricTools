// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "ClipMesh.h"
#include "PartitionMesh.h"

WM5_WINDOW_APPLICATION(ClipMesh);

//----------------------------------------------------------------------------
ClipMesh::ClipMesh ()
    :
    WindowApplication3("SampleMathematics/ClipMesh", 0, 0, 640, 480,
        Float4(0.75f, 0.75f, 0.75f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
}
//----------------------------------------------------------------------------
bool ClipMesh::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 0.1f, 1000.0f);
    APoint camPosition(16.0f, 0.0f, 4.0f);
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

    InitializeCameraMotion(0.01f, 0.001f);
    InitializeObjectMotion(mTorus);
    return true;
}
//----------------------------------------------------------------------------
void ClipMesh::OnTerminate ()
{
    mScene = 0;
    mTorus = 0;
    mMeshPlane = 0;
    mTorusWireState = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void ClipMesh::OnIdle ()
{
    MeasureTime();

    if (MoveCamera())
    {
        mCuller.ComputeVisibleSet(mScene);
    }

    if (MoveObject())
    {
        Update();
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
bool ClipMesh::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication3::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
    case 'w':
    case 'W':
        mTorusWireState->Enabled = !mTorusWireState->Enabled;
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void ClipMesh::CreateScene ()
{
    mScene = new0 Node();

    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);

    // The plane is fixed at z = 0.
    mPlane.SetNormal(AVector::UNIT_Z);
    mPlane.SetConstant(0.0f);
    mMeshPlane = StandardMesh(vformat).Rectangle(32, 32, 16.0f, 16.0f);
    VisualEffectInstance* instance =
        VertexColor3Effect::CreateUniqueInstance();
    instance->GetEffect()->GetWireState(0,0)->Enabled = true;
    mMeshPlane->SetEffectInstance(instance);
    mScene->AttachChild(mMeshPlane);

    VertexBufferAccessor vba(mMeshPlane);
    Float3 green(0.0f, 1.0f, 0.0f);
    int i;
    for (i = 0; i < vba.GetNumVertices(); ++i)
    {
        vba.Color<Float3>(0, i) = green;
    }

    // Get the positions and indices for a torus.
    mTorus = StandardMesh(vformat).Torus(64, 64, 4.0f, 1.0f);
    instance = VertexColor3Effect::CreateUniqueInstance();
    mTorusWireState = instance->GetEffect()->GetWireState(0, 0);
    mTorus->SetEffectInstance(instance);
    mScene->AttachChild(mTorus);

    vba.ApplyTo(mTorus);
    mTorusVerticesMS.resize(vba.GetNumVertices());
    mTorusVerticesWS.resize(vba.GetNumVertices());
    Float3 black(0.0f, 0.0f, 0.0f);
    for (i = 0; i < vba.GetNumVertices(); ++i)
    {
        mTorusVerticesMS[i] = vba.Position<Float3>(i);
        mTorusVerticesWS[i] = mTorusVerticesMS[i];
        vba.Color<Float3>(0, i) = black;
    }

    IndexBuffer* ibuffer = mTorus->GetIndexBuffer();
    int numIndices = ibuffer->GetNumElements();
    int* indices = (int*)ibuffer->GetData();
    mTorusIndices.resize(numIndices);
    memcpy(&mTorusIndices[0], indices, numIndices*sizeof(int));

    Update();
}
//----------------------------------------------------------------------------
void ClipMesh::Update ()
{
    // Transform the model-space vertices to world space.
    int numVertices = (int)mTorusVerticesMS.size();
    int i;
    for (i = 0; i < numVertices; ++i)
    {
        mTorusVerticesWS[i] = mTorus->LocalTransform*mTorusVerticesMS[i];
    }

    // Partition the torus mesh.
    std::vector<APoint> clipVertices;
    std::vector<int> negIndices, posIndices;
    PartitionMesh(mTorusVerticesWS, mTorusIndices, mPlane, clipVertices,
        negIndices, posIndices);

    // Replace the torus vertex buffer.
    numVertices = (int)clipVertices.size();
    int stride = mTorus->GetVertexFormat()->GetStride();
    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, stride,
        Buffer::BU_STATIC);
    mTorus->SetVertexBuffer(vbuffer);
    VertexBufferAccessor vba(mTorus);
    Float3 black(0.0f, 0.0f, 0.0f);
    for (i = 0; i < numVertices; ++i)
    {
        // Transform the world-space vertex to model space.
        vba.Position<Float3>(i) =
            mTorus->LocalTransform.Inverse()*clipVertices[i];

        vba.Color<Float3>(0, i) = black;
    }

    // Modify the vertex color based on which mesh the vertices lie.
    int negQuantity = (int)negIndices.size();
    for (i = 0; i < negQuantity; ++i)
    {
        vba.Color<Float3>(0, negIndices[i])[2] = 1.0f;
    }
    int posQuantity = (int)posIndices.size();
    for (i = 0; i < posQuantity; ++i)
    {
        vba.Color<Float3>(0, posIndices[i])[0] = 1.0f;
    }

    // To display the triangles generated by the split.
    int numIndices = negQuantity + posQuantity;
    IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, sizeof(int));
    mTorus->SetIndexBuffer(ibuffer);
    int* indices = (int*)ibuffer->GetData();
    memcpy(indices, &negIndices[0], negQuantity*sizeof(int));
    memcpy(indices + negQuantity, &posIndices[0], posQuantity*sizeof(int));
}
//----------------------------------------------------------------------------
