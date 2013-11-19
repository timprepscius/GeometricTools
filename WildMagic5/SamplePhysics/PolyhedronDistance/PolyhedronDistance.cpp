// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2012/08/04)

#include "PolyhedronDistance.h"

WM5_WINDOW_APPLICATION(PolyhedronDistance);

//----------------------------------------------------------------------------
PolyhedronDistance::PolyhedronDistance ()
    :
    WindowApplication3("SamplePhysics/PolyhedronDistance", 0, 0, 640, 480,
        Float4(0.5f, 0.0f, 1.0f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
}
//----------------------------------------------------------------------------
bool PolyhedronDistance::OnInitialize()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 1000.0f);
    APoint camPosition(0.0f, -2.0f, 0.5f);
    AVector camDVector(0.0f, 1.0f, 0.0f);
    AVector camUVector(0.0f, 0.0f, 1.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    CreateScene();

    // Initial update of objects.
    mScene->Update();
    UpdateSegments();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.01f, 0.01f);
    return true;
}
//----------------------------------------------------------------------------
void PolyhedronDistance::OnTerminate()
{
    delete1(mFaces);
    mScene = 0;
    mWireState = 0;
    mSegments[0] = 0;
    mSegments[1] = 0;
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
bool PolyhedronDistance::OnKeyDown (unsigned char key, int x, int y)
{
    if (ApplyTransform(key))
    {
        mCuller.ComputeVisibleSet(mScene);
        return true;
    }

    switch (key)
    {
    case 'w':  // toggle wireframe
        mWireState->Enabled = !mWireState->Enabled;
        return true;
    }

    return WindowApplication3::OnKeyDown(key, x, y);
}
//----------------------------------------------------------------------------
void PolyhedronDistance::OnIdle ()
{
    if (MoveCamera())
    {
        mCuller.ComputeVisibleSet(mScene);
    }

    if (mRenderer->PreDraw())
    {
        mRenderer->ClearBuffers();
        mRenderer->Draw(mCuller.GetVisibleSet());

        char message[256];
        sprintf(message, "separation = %3.2f",
            mSeparation/(Mathf::Sqrt(2.0)*mEdgeLength));
        mRenderer->Draw(8, GetHeight()-8, mTextColor, message);

        sprintf(message, " small tetrahedron sides." );
        mRenderer->Draw(140, GetHeight()-8, mTextColor, message);

        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }
}
//----------------------------------------------------------------------------
void PolyhedronDistance::CreateScene ()
{
    // ** layout of scene graph **
    // scene
    //     tetra[4]
    //     plane
    //     line[2]

    // Create the objects.
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    TriMesh* plane = CreatePlane();
    int i;
    for (i = 0; i < 2; ++i)
    {
        // Build the display tetrahedra.
        float size = 0.3f + 0.2f*(i + 1);
        if (i == 0)
        {
            mEdgeLength = size;
        }
        mTetras[i] = CreateTetra(size, false);
        mSegments[i] = CreateSegment();

        // Build the point tetrahedra.
        mSmall = 0.02f;
        mTetras[i + 2] = CreateTetra(mSmall, true);
    }

    // Tetrahedra faces.
    mFaces = new1<Tuple<3,int> >(4);
    mFaces[0][0] = 1;  mFaces[0][1] = 2;  mFaces[0][2] = 0;
    mFaces[1][0] = 0;  mFaces[1][1] = 3;  mFaces[1][2] = 2;
    mFaces[2][0] = 0;  mFaces[2][1] = 1;  mFaces[2][2] = 3;
    mFaces[3][0] = 1;  mFaces[3][1] = 2;  mFaces[3][2] = 3;

    // Transform the tetrahedra.
    mTetras[0]->LocalTransform.SetRotate(HMatrix(AVector::UNIT_Z, 1.1f));
    mTetras[0]->LocalTransform.SetTranslate(APoint(-0.25f, 0.1f, 0.3f));
    mTetras[1]->LocalTransform.SetRotate(HMatrix(AVector::UNIT_Z, 0.3f));
    mTetras[1]->LocalTransform.SetTranslate(APoint(0.25f, 0.4f, 0.5f));

    // Set parent-child links.
    mScene->AttachChild(plane);
    for (i = 0; i < 2; ++i)
    {
        mScene->AttachChild(mTetras[i]);
        mScene->AttachChild(mSegments[i]);
        mScene->AttachChild(mTetras[i + 2]);
    }
}
//----------------------------------------------------------------------------
TriMesh* PolyhedronDistance::CreateTetra (float size, bool isBlack)
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(4, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);

    vba.Position<Vector3f>(0) = -(size/3.0f)*Vector3f(1.0f, 1.0f, 1.0f);
    vba.Position<Vector3f>(1) = Vector3f(size, 0.0f, 0.0f);
    vba.Position<Vector3f>(2) = Vector3f(0.0f, size, 0.0f);
    vba.Position<Vector3f>(3) = Vector3f(0.0f, 0.0f, size);

    if (isBlack)
    {
        // Black tetrahedra for the small ones used as points.
        Float3 black(0.0f, 0.0f, 0.0f);
        vba.Color<Float3>(0, 0) = black;
        vba.Color<Float3>(0, 1) = black;
        vba.Color<Float3>(0, 2) = black;
        vba.Color<Float3>(0, 3) = black;
    }
    else
    {
        // Colorful colors for the tetrahedra under study.
        vba.Color<Float3>(0, 0) = Float3(0.0f, 0.0f, 1.0f);
        vba.Color<Float3>(0, 1) = Float3(0.0f, 1.0f, 0.0f);
        vba.Color<Float3>(0, 2) = Float3(1.0f, 0.0f, 0.0f);
        vba.Color<Float3>(0, 3) = Float3(1.0f, 1.0f, 1.0f);
    }

    IndexBuffer* ibuffer = new0 IndexBuffer(12, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[ 0] = 0; indices[ 1] = 2; indices[ 2] = 1;
    indices[ 3] = 0; indices[ 4] = 3; indices[ 5] = 2;
    indices[ 6] = 0; indices[ 7] = 1; indices[ 8] = 3;
    indices[ 9] = 1; indices[10] = 2; indices[11] = 3;

    TriMesh* mesh = new0 TriMesh(vformat, vbuffer, ibuffer);
    mesh->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());

    return mesh;
}
//----------------------------------------------------------------------------
Polysegment* PolyhedronDistance::CreateSegment ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(2, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);

    vba.Position<Vector3f>(0) = Vector3f::ZERO;
    vba.Position<Vector3f>(1) = Vector3f::UNIT_X;
    vba.Color<Float3>(0, 0) = Float3(1.0f, 1.0f, 1.0f);
    vba.Color<Float3>(0, 1) = Float3(1.0f, 1.0f, 1.0f);

    Polysegment* segment = new0 Polysegment(vformat, vbuffer, true);
    segment->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());

    return segment;
}
//----------------------------------------------------------------------------
void PolyhedronDistance::UpdateSegments ()
{
    // Two segments make the line easier to see.
    Vector3f U[2][4];

    // Offset the polyhedra so far into the first octant that we are unlikely
    // to translate them out of that octant during a run.
    mOffsetMagnitude = 20.0f;
    Vector3f offset = mOffsetMagnitude*Vector3f::ONE;

    VertexBufferAccessor vba;
    int i;
    for (i = 0; i < 2; ++i)
    {
        const APoint& wTrn = mTetras[i]->WorldTransform.GetTranslate();
        const HMatrix& wRot = mTetras[i]->WorldTransform.GetRotate();
        vba.ApplyTo(mTetras[i]);
        for (int j = 0; j < 4; ++j)
        {
            AVector relPosition = vba.Position<Float3>(j);
            APoint position = wTrn + wRot*relPosition;
            U[i][j] = Vector3f(position[0] + offset[0],
                position[1] + offset[1], position[2] + offset[2]);
        }
    }

    vba.ApplyTo(mSegments[0]);
    Vector3f vertex[2] =
    {
        vba.Position<Vector3f>(0),
        vba.Position<Vector3f>(1)
    };

    int statusCode;

    LCPPolyDist3(4, U[0], 4, mFaces, 4, U[1], 4, mFaces, statusCode,
        mSeparation, vertex);

    vba.Position<Vector3f>(0) = vertex[0];
    vba.Position<Vector3f>(1) = vertex[1];

    if ((statusCode != LCPPolyDist3::SC_FOUND_SOLUTION &&
        statusCode != LCPPolyDist3::SC_TEST_POINTS_TEST_FAILED &&
        statusCode != LCPPolyDist3::SC_FOUND_TRIVIAL_SOLUTION) ||
        mSeparation < 0.0f)
    {
        // Do not draw the line joining nearest points if returns from
        // LCPPolyDist are not appropriate.
        for (i = 0; i < 2; ++i)
        {
            vba.Position<Vector3f>(i) = -offset;
        }
    }

    // Correct for the offset and set up endpoints for the segment.
    for (i = 0; i < 2; ++i)
    {
        vba.Position<Vector3f>(i) -= offset;

        // The adjustment with mSmall "centers" the endpoint tetra on the
        // solution points.
        Vector3f temp = vba.Position<Vector3f>(i) -
            (mSmall/3.0f)*Vector3f::ONE;
        mTetras[i + 2]->LocalTransform.SetTranslate(temp);
    }

    // Double-up the line for better visibility.
    vertex[0] = vba.Position<Vector3f>(0);
    vertex[1] = vba.Position<Vector3f>(1);
    vba.ApplyTo(mSegments[1]);
    const float epsilon = 0.002f;
    for (i = 0; i < 2; ++i)
    {
        vba.Position<Vector3f>(i) = vertex[i] + epsilon*Vector3f::ONE;
    }

    for (i = 0; i < 2; ++i)
    {
        mSegments[i]->UpdateModelSpace(Visual::GU_MODEL_BOUND_ONLY);
        mSegments[i]->Update();
        mRenderer->Update(mSegments[i]->GetVertexBuffer());
    }

    mScene->Update();
}
//----------------------------------------------------------------------------
TriMesh* PolyhedronDistance::CreatePlane ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(4, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);

    float size = 16.0f;
    vba.Position<Float3>(0) = Float3(-size, -size, -0.1f);
    vba.Position<Float3>(1) = Float3(+size, -size, -0.1f);
    vba.Position<Float3>(2) = Float3(+size, +size, -0.1f);
    vba.Position<Float3>(3) = Float3(-size, +size, -0.1f);
    vba.Color<Float3>(0, 0) = Float3(0.0f, 0.50f, 0.00f);
    vba.Color<Float3>(0, 1) = Float3(0.0f, 0.25f, 0.00f);
    vba.Color<Float3>(0, 2) = Float3(0.0f, 0.75f, 0.00f);
    vba.Color<Float3>(0, 3) = Float3(0.0f, 1.00f, 0.00f);

    IndexBuffer* ibuffer = new0 IndexBuffer(6, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[0] = 0; indices[1] = 1; indices[2] = 2;
    indices[3] = 0; indices[4] = 2; indices[5] = 3;

    TriMesh* mesh = new0 TriMesh(vformat, vbuffer, ibuffer);
    mesh->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());

    return mesh;
}
//----------------------------------------------------------------------------
bool PolyhedronDistance::ApplyTransform (unsigned char key)
{
    APoint trn;
    HMatrix rot, incr;
    float trnSpeed = 0.1f;
    float rotSpeed = 0.1f;

    switch (key)
    {
        case 'x':
            trn = mTetras[0]->LocalTransform.GetTranslate();
            trn.X() -= trnSpeed;
            mTetras[0]->LocalTransform.SetTranslate(trn);
            break;
        case 'X':
            trn = mTetras[0]->LocalTransform.GetTranslate();
            trn.X() += trnSpeed;
            mTetras[0]->LocalTransform.SetTranslate(trn);
            break;
        case 'y':
            trn = mTetras[0]->LocalTransform.GetTranslate();
            trn.Y() -= trnSpeed;
            mTetras[0]->LocalTransform.SetTranslate(trn);
            break;
        case 'Y':
            trn = mTetras[0]->LocalTransform.GetTranslate();
            trn.Y() += trnSpeed;
            mTetras[0]->LocalTransform.SetTranslate(trn);
            break;
        case 'z':
            trn = mTetras[0]->LocalTransform.GetTranslate();
            trn.Z() -= trnSpeed;
            mTetras[0]->LocalTransform.SetTranslate(trn);
            break;
        case 'Z':
            trn = mTetras[0]->LocalTransform.GetTranslate();
            trn.Z() += trnSpeed;
            mTetras[0]->LocalTransform.SetTranslate(trn);
            break;
        case 's':
            trn = mTetras[1]->LocalTransform.GetTranslate();
            trn.X() -= trnSpeed;
            mTetras[1]->LocalTransform.SetTranslate(trn);
            break;
        case 'S':
            trn = mTetras[1]->LocalTransform.GetTranslate();
            trn.X() += trnSpeed;
            mTetras[1]->LocalTransform.SetTranslate(trn);
            break;
        case 't':
            trn = mTetras[1]->LocalTransform.GetTranslate();
            trn.Y() -= trnSpeed;
            mTetras[1]->LocalTransform.SetTranslate(trn);
            break;
        case 'T':
            trn = mTetras[1]->LocalTransform.GetTranslate();
            trn.Y() += trnSpeed;
            mTetras[1]->LocalTransform.SetTranslate(trn);
            break;
        case 'u':
            trn = mTetras[1]->LocalTransform.GetTranslate();
            trn.Z() -= trnSpeed;
            mTetras[1]->LocalTransform.SetTranslate(trn);
            break;
        case 'U':
            trn = mTetras[1]->LocalTransform.GetTranslate();
            trn.Z() += trnSpeed;
            mTetras[1]->LocalTransform.SetTranslate(trn);
            break;
        case 'a':
            rot = mTetras[0]->LocalTransform.GetRotate();
            incr.MakeRotation(AVector::UNIT_Y, rotSpeed);
            mTetras[0]->LocalTransform.SetRotate(incr*rot);
            break;
        case 'A':
            rot = mTetras[0]->LocalTransform.GetRotate();
            incr.MakeRotation(AVector::UNIT_Y, -rotSpeed);
            mTetras[0]->LocalTransform.SetRotate(incr*rot);
            break;
        case 'b':
            rot = mTetras[1]->LocalTransform.GetRotate();
            incr.MakeRotation(AVector::UNIT_Z, rotSpeed);
            mTetras[1]->LocalTransform.SetRotate(incr*rot);
            break;
        case 'B':
            rot = mTetras[1]->LocalTransform.GetRotate();
            incr.MakeRotation(AVector::UNIT_Z, -rotSpeed);
            mTetras[1]->LocalTransform.SetRotate(incr*rot);
            break;
        case 'c':
            rot = mTetras[0]->LocalTransform.GetRotate();
            incr.MakeRotation(AVector::UNIT_X, 1.3f*rotSpeed);
            mTetras[0]->LocalTransform.SetRotate(incr*rot);
            rot = mTetras[1]->LocalTransform.GetRotate();
            incr.MakeRotation(AVector::UNIT_Z, -rotSpeed);
            mTetras[1]->LocalTransform.SetRotate(incr*rot);
            break;
        case 'C':
            rot = mTetras[0]->LocalTransform.GetRotate();
            incr.MakeRotation(AVector::UNIT_X, -1.3f*rotSpeed);
            mTetras[0]->LocalTransform.SetRotate(incr*rot);
            rot = mTetras[1]->LocalTransform.GetRotate();
            incr.MakeRotation(AVector::UNIT_Z, rotSpeed);
            mTetras[1]->LocalTransform.SetRotate(incr*rot);
            break;
        default:
            return false;
    }

    // Prevent solution point coordinates from being negative.  The polyhedron
    // distance calculator expects solution points to be in the first octant.
    // Vertices are offset by mOffsetMagnitude*Vector3f(1,1,1) in
    // UpdateSegments() before the call to the distance routine.  Here we make
    // sure that no translation of a polyhedron takes it so far into one of the
    // other 7 octants that the offset will not be sufficient to guarantee that
    // the solution points lie in the first octant.
    VertexBufferAccessor vba;
    float threshold = -mOffsetMagnitude + trnSpeed;
    for (int j = 0; j < 2; ++j)
    {
        const APoint& wTrn = mTetras[j]->WorldTransform.GetTranslate();
        const HMatrix& wRot = mTetras[j]->WorldTransform.GetRotate();
        vba.ApplyTo(mTetras[j]);
        for (int i = 0; i < 4; i++)
        {
            AVector relPosition = vba.Position<Float3>(i);
            APoint position = wTrn + wRot*relPosition;
            APoint trn = mTetras[j]->LocalTransform.GetTranslate();
            for (int k = 0; k < 3; ++k)
            {
                if (position[k] < threshold)
                {
                    trn[k] += trnSpeed;
                }
            }
            mTetras[j]->LocalTransform.SetTranslate(trn);
        }
    }

    mScene->Update();
    UpdateSegments();
    return true;
}
//----------------------------------------------------------------------------
