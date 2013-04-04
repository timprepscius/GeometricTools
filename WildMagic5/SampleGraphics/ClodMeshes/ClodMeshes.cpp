// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "ClodMeshes.h"

WM5_WINDOW_APPLICATION(ClodMeshes);

//----------------------------------------------------------------------------
ClodMeshes::ClodMeshes ()
    :
    WindowApplication3("SampleGraphics/ClodMeshes", 0, 0, 640, 480,
        Float4(0.9f, 0.9f, 0.9f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
}
//----------------------------------------------------------------------------
bool ClodMeshes::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();

    // Center-and-fit scene in the view frustum.
    mScene->Update();
    mTrnNode->LocalTransform.SetTranslate(-mScene->WorldBound.GetCenter());
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 10000.0f);
    AVector camDVector(0.0f, 1.0f, 0.0f);
    AVector camUVector(0.0f, 0.0f, 1.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    APoint camPosition = APoint::ORIGIN -
        3.0f*mScene->WorldBound.GetRadius()*camDVector;
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(1.0f, 0.001f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void ClodMeshes::OnTerminate ()
{
    mScene = 0;
    mTrnNode = 0;
    mWireState = 0;

#ifdef USE_CLOD_MESH
    mClod[0] = 0;
    mClod[1] = 0;
    mActive = 0;
#endif

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void ClodMeshes::OnIdle ()
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

#ifdef USE_CLOD_MESH
        char message[256];
        sprintf(message, "triangles0: %4d   triangles1: %4d",
            mClod[0]->GetNumTriangles(), mClod[1]->GetNumTriangles());
        mRenderer->Draw(128, GetHeight()-8, mTextColor, message);
#endif

        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool ClodMeshes::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication3::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
#ifdef USE_CLOD_MESH
    case '0':
        mActive = mClod[0];
        return true;
    case '1':
        mActive = mClod[1];
        return true;

    case '-':  // decrease triangle quantity
    case '_':
        if (mActive->TargetRecord() < mActive->GetNumRecords() - 1)
        {
            ++mActive->TargetRecord();
        }
        mActive->Update();
        mCuller.ComputeVisibleSet(mScene);
        return true;

    case '+':  // increase triangle quantity
    case '=':
        if (mActive->TargetRecord() > 0)
        {
            --mActive->TargetRecord();
        }
        mActive->Update();
        mCuller.ComputeVisibleSet(mScene);
        return true;

    case 'c':  // toggle between 1/4 and full resolution
    case 'C':
        if (mActive->TargetRecord() != 0)
        {
            mActive->TargetRecord() = 0;
        }
        else
        {
            mActive->TargetRecord() = 3*(mActive->GetNumRecords() - 1)/4;
        }
        mActive->Update();
        mCuller.ComputeVisibleSet(mScene);
        return true;
#endif

    case 'w':
    case 'W':
        mWireState->Enabled = !mWireState->Enabled;
        return true;
     }

    return false;
}
//----------------------------------------------------------------------------
void ClodMeshes::CreateScene ()
{
    mScene = new0 Node();
    mTrnNode = new0 Node();
    mScene->AttachChild(mTrnNode);
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    // Load the face model.
#ifdef WM5_LITTLE_ENDIAN
    std::string path = Environment::GetPathR("FacePN.wmof");
#else
    std::string path = Environment::GetPathR("FacePN.be.wmof");
#endif
    InStream inStream;
    inStream.Load(path);
    TriMeshPtr mesh = StaticCast<TriMesh>(inStream.GetObjectAt(0));
    VertexBufferAccessor vba0(mesh);

    // Remove the normals and add texture coordinates.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(vba0.GetNumVertices(), vstride);
    VertexBufferAccessor vba1(vformat, vbuffer);

    float xmin = Mathf::MAX_REAL, xmax = -Mathf::MAX_REAL;
    float ymin = Mathf::MAX_REAL, ymax = -Mathf::MAX_REAL;
    int i;
    for (i = 0; i < vba0.GetNumVertices(); ++i)
    {
        Float3 position = vba0.Position<Float3>(i);
        vba1.Position<Float3>(i) = position;

        float x = position[0];
        float y = position[2];
        vba1.TCoord<Float2>(0, i) = Float2(x, y);

        if (x < xmin)
        {
            xmin = x;
        }
        if (x > xmax)
        {
            xmax = x;
        }
        if (y < ymin)
        {
            ymin = y;
        }
        if (y > ymax)
        {
            ymax = y;
        }
    }

    float xmult = 1.0f/(xmax - xmin);
    float ymult = 1.0f/(ymax - ymin);
    for (i = 0; i < vba1.GetNumVertices(); ++i)
    {
        Float2 tcoord = vba1.TCoord<Float2>(0, i);
        vba1.TCoord<Float2>(0,i) = Float2(
            (tcoord[0] - xmin)*xmult,
            (tcoord[1] - ymin)*ymult);
    }

    mesh->SetVertexFormat(vformat);
    mesh->SetVertexBuffer(vbuffer);

    // Create a texture for the face.  Use the generated texture coordinates.
    Texture2DEffect* effect = new0 Texture2DEffect(Shader::SF_LINEAR);
    path = Environment::GetPathR("Magician.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);

#ifdef USE_CLOD_MESH
    // Create the collapse records to be shared by two CLOD meshes.
    int numRecords = 0;
    CollapseRecord* records = 0;
    CreateClodMesh ccm(mesh, numRecords, records);
    CollapseRecordArray* recordArray = new0 CollapseRecordArray(numRecords,
        records);

    mClod[0] = new0 ClodMesh(mesh, recordArray);
    mClod[0]->LocalTransform = mesh->LocalTransform;
    mClod[0]->LocalTransform.SetTranslate(mesh->LocalTransform.GetTranslate()
        - 150.0f*AVector::UNIT_X);
    mClod[0]->SetEffectInstance(effect->CreateInstance(texture));
    mTrnNode->AttachChild(mClod[0]);

    mClod[1] = new0 ClodMesh(mesh, recordArray);
    mClod[1]->LocalTransform = mesh->LocalTransform;
    mClod[1]->LocalTransform.SetTranslate(mesh->LocalTransform.GetTranslate()
        + 150.0f*AVector::UNIT_X - 100.0f*AVector::UNIT_Y);
    mClod[1]->SetEffectInstance(effect->CreateInstance(texture));
    mTrnNode->AttachChild(mClod[1]);

    mActive = mClod[0];
#else
    IndexBuffer* ibuffer = mesh->GetIndexBuffer();
    TriMesh* face = new0 TriMesh(vformat, vbuffer,ibuffer);
    face->LocalTransform = mesh->LocalTransform;
    face->LocalTransform.SetTranslate(mesh->LocalTransform.GetTranslate() -
        150.0f*AVector::UNIT_X);
    face->SetEffectInstance(effect->CreateInstance(texture));
    mTrnNode->AttachChild(face);

    face = new0 TriMesh(vformat, vbuffer, ibuffer);
    face->LocalTransform = mesh->LocalTransform;
    face->LocalTransform.SetTranslate(mesh->LocalTransform.GetTranslate() +
        150.0f*AVector::UNIT_X);
    face->SetEffectInstance(effect->CreateInstance(texture));
    mTrnNode->AttachChild(face);
#endif
}
//----------------------------------------------------------------------------
#ifdef USE_CLOD_MESH
//----------------------------------------------------------------------------
void ClodMeshes::MoveForward ()
{
    WindowApplication3::MoveForward();
    UpdateClods();
}
//----------------------------------------------------------------------------
void ClodMeshes::MoveBackward ()
{
    WindowApplication3::MoveBackward();
    UpdateClods();
}
//----------------------------------------------------------------------------
void ClodMeshes::UpdateClods ()
{
    // Adjust the triangle quantities for the clod meshes.  The distance along
    // the camera direction controls the triangle quantities.  A nonlinear
    // drop-off is used.
    for (int i = 0; i < 2; ++i)
    {
        AVector diff =
            mClod[i]->WorldBound.GetCenter() - mCamera->GetPosition();

        float depth = diff.Dot(mCamera->GetDVector());
        int targetRecord;
        if (depth <= mCamera->GetDMin())
        {
            targetRecord = 0;
        }
        else if (depth >= mCamera->GetDMax())
        {
            targetRecord = mClod[i]->GetNumRecords() - 1;
        }
        else
        {
            float dmin = mCamera->GetDMin();
            float dmax = mCamera->GetDMax();
            float ratio = Mathf::Pow((depth - dmin)/(dmax - dmin), 0.5f);
            targetRecord = (int)((mClod[i]->GetNumRecords() - 1)*ratio);
        }

        mClod[i]->TargetRecord() = targetRecord;
    }
}
//----------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------
