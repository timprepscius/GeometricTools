// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2012/07/07)

#include "ConvexHull3D.h"
#include "Wm5ConvexHull3.h"

WM5_WINDOW_APPLICATION(ConvexHull3D);

//----------------------------------------------------------------------------
ConvexHull3D::ConvexHull3D ()
    :
    WindowApplication3("SampleMathematics/ConvexHull3D", 0, 0, 640, 480,
        Float4(1.0f, 1.0f, 1.0f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    Environment::InsertDirectory(ThePath + "Data/");

    mFileQuantity = 46;
    mCurrentFile = 1;
    mNumVertices = 0;
    mVertices = 0;
    mColors = 0;
    mLimitedQuantity = 0;
    mQueryType = Query::QT_INT64;
    mHull = 0;
    strcpy(mHeader, "query type = INT64");
}
//----------------------------------------------------------------------------
bool ConvexHull3D::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // The scene creation involves culling, so mCuller needs to know its
    // camera now.
    mCuller.SetCamera(mCamera);
    CreateScene();

    // Center-and-fit for camera viewing.
    mScene->Update();
    mTrnNode->LocalTransform.SetTranslate(-mScene->WorldBound.GetCenter());
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 10000.0f);
    AVector camDVector(0.0f, 0.0f, 1.0f);
    AVector camUVector(0.0f, 1.0f, 0.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    APoint camPosition = APoint::ORIGIN -
        2.5f*mScene->WorldBound.GetRadius()*camDVector;
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(1.0f, 0.01f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void ConvexHull3D::OnTerminate ()
{
    mPicker.Records.clear();

    delete1(mVertices);
    delete1(mColors);
    delete0(mHull);

    mScene = 0;
    mHullNode = 0;
    mSphere = 0;
    mWireState = 0;
    mCullState = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void ConvexHull3D::OnIdle ()
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
        mRenderer->Draw(8, 16, mTextColor, mHeader);
        mRenderer->Draw(8, GetHeight()-8, mTextColor, mFooter);
        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool ConvexHull3D::OnKeyDown (unsigned char key, int x, int y)
{
    switch (key)
    {
    // load a new data set
    case 'd':
    case 'D':
        if (++mCurrentFile == mFileQuantity)
        {
            mCurrentFile = 1;
        }

        LoadData();
        return true;

    // query type INT64
    case 'n':
    case 'N':
        mQueryType = Query::QT_INT64;
        strcpy(mHeader, "query type = INT64");
        RegenerateHull();
        return true;

    // query type INTEGER
    case 'i':
    case 'I':
        mQueryType = Query::QT_INTEGER;
        strcpy(mHeader, "query type = INTEGER");
        RegenerateHull();
        return true;

    // query type RATIONAL
    case 'r':
    case 'R':
        mQueryType = Query::QT_RATIONAL;
        strcpy(mHeader, "query type = RATIONAL");
        RegenerateHull();
        return true;

    // query type REAL (float)
    case 'f':
    case 'F':
        mQueryType = Query::QT_REAL;
        strcpy(mHeader, "query type = REAL");
        RegenerateHull();
        return true;

    // query type FILTERED
    case 'c':
    case 'C':
        mQueryType = Query::QT_FILTERED;
        strcpy(mHeader, "query type = FILTERED");
        RegenerateHull();
        return true;

    case 'w':
    case 'W':
        mWireState->Enabled = !mWireState->Enabled;
        return true;

    // Read the notes in ConvexHul3D.h about how to use mLimitedQuantity.
    case '+':
    case '=':
        if (mLimitedQuantity < mNumVertices)
        {
            for (int i = 2; i < mLimitedQuantity + 2; ++i)
            {
                mTrnNode->DetachChildAt(i);
            }
            ++mLimitedQuantity;
            CreateHull();
        }
        return true;
    case '-':
    case '_':
        if (mLimitedQuantity > 3)
        {
            for (int i = 2; i < mLimitedQuantity + 2; ++i)
            {
                mTrnNode->DetachChildAt(i);
            }
            --mLimitedQuantity;
            CreateHull();
        }
        return true;
    }

    return WindowApplication::OnKeyDown(key, x, y);
}
//----------------------------------------------------------------------------
bool ConvexHull3D::OnMouseClick (int button, int state, int x, int y,
    unsigned int modifiers)
{
    WindowApplication3::OnMouseClick(button, state, x, y, modifiers);

    if (button == MOUSE_RIGHT_BUTTON)
    {
        // Convert to right-handed screen coordinates.
        y = GetHeight() - 1 - y;

        APoint origin;
        AVector direction;
        mRenderer->GetPickRay(x, y, origin, direction);
        mPicker.Execute(mTrnNode, origin, direction, 0.0f, Mathf::MAX_REAL);
        if (mPicker.Records.size() > 0)
        {
            const PickRecord& record = mPicker.GetClosestNonnegative();
            TriMeshPtr mesh = StaticCast<TriMesh>(record.Intersected);
            float maxBary = record.Bary[0];
            int index = 0;
            if (record.Bary[1] > maxBary)
            {
                maxBary = record.Bary[1];
                index = 1;
            }
            if (record.Bary[2] > maxBary)
            {
                maxBary = record.Bary[2];
                index = 2;
            }
            int* indices =(int*) mesh->GetIndexBuffer()->GetData();
            sprintf(mFooter, "intr = %d, tri = %d, ver = %d",
                (int)mPicker.Records.size(), record.Triangle,
                indices[3*record.Triangle + index]);
        }
    }

    return true;
}
//----------------------------------------------------------------------------
void ConvexHull3D::CreateScene ()
{
    mScene = new0 Node();
    mTrnNode = new0 Node();
    mScene->AttachChild(mTrnNode);
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);
    mCullState = new0 CullState();
    mCullState->Enabled = false;
    mRenderer->SetOverrideCullState(mCullState);

    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);

    TriMesh* sphere = StandardMesh(vformat).Sphere(8, 8, 0.01f);
    sphere->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());
    mTrnNode->SetChild(1, sphere);

    // The current file is "Data/data01.txt".
    LoadData();
}
//----------------------------------------------------------------------------
void ConvexHull3D::LoadData ()
{
    char filename[32];
    if (mCurrentFile < 10)
    {
        sprintf(filename, "data0%d.txt", mCurrentFile);
    }
    else
    {
        sprintf(filename, "data%d.txt", mCurrentFile);
    }

    std::string path = Environment::GetPathR(filename);
    std::ifstream inFile(path.c_str());
    assert(inFile);
    inFile >> mNumVertices;

    delete1(mVertices);
    mVertices = new1<Vector3f>(mNumVertices);
    int i;
    for (i = 0; i < mNumVertices; ++i)
    {
        inFile >> mVertices[i][0];
        inFile >> mVertices[i][1];
        inFile >> mVertices[i][2];
    }

    delete1(mColors);
    mColors = new1<Float3>(mNumVertices);
    for (i = 0; i < mNumVertices; ++i)
    {
        mColors[i] = Float3(Mathf::UnitRandom(), Mathf::UnitRandom(),
            Mathf::UnitRandom());
    }

    // Discard previous scene spheres.
    for (i = 2; i < mLimitedQuantity + 2; ++i)
    {
        mTrnNode->DetachChildAt(i);
    }

    mLimitedQuantity = mNumVertices;
    CreateHull();
}
//----------------------------------------------------------------------------
void ConvexHull3D::CreateHull ()
{
    int numVertices = mLimitedQuantity;

    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);
    int i;
    for (i = 0; i < numVertices; ++i)
    {
        vba.Position<Vector3f>(i) = mVertices[i];
        vba.Color<Float3>(0, i) = mColors[i];
    }

    RegenerateHull();

    int numTriangles = 0;
    TriMesh* mesh = 0;

    switch (mHull->GetDimension())
    {
    case 0:
        sprintf(mFooter, "point: v = %d, t = %d", numVertices, numTriangles);
        return;
    case 1:
        sprintf(mFooter, "linear: v = %d, t = %d", numVertices, numTriangles);
        return;
    case 2:
    {
        numTriangles = mHull->GetNumSimplices() - 2;
        const int* hullIndices = mHull->GetIndices();
        IndexBuffer* ibuffer = new0 IndexBuffer(3*numTriangles, sizeof(int));
        int* indices = (int*)ibuffer->GetData();
        for (int t = 0, i0 = 1, i1 = 2; t < numTriangles; ++t, ++i0, ++i1)
        {
            *indices++ = hullIndices[0];
            *indices++ = hullIndices[i0];
            *indices++ = hullIndices[i1];
        }
        mesh = new0 TriMesh(vformat, vbuffer, ibuffer);
        mesh->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());

        sprintf(mFooter, "planar: v = %d, t = %d", numVertices, numTriangles);
        break;
    }
    case 3:
        numTriangles = mHull->GetNumSimplices();
        const int* hullIndices = mHull->GetIndices();
        IndexBuffer* ibuffer = new0 IndexBuffer(3*numTriangles, sizeof(int));
        int* indices = (int*)ibuffer->GetData();
        memcpy(indices, hullIndices, 3*numTriangles*sizeof(int));
        mesh = new0 TriMesh(vformat, vbuffer, ibuffer);
        mesh->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());

        sprintf(mFooter, "spatial: v = %d, t = %d", numVertices,
            numTriangles);
        break;
    }

    // Translate to center of mass.
    Vector3f center = mVertices[0];
    for (i = 1; i < mLimitedQuantity; ++i)
    {
        center += mVertices[i];
    }
    center /= (float)mLimitedQuantity;
    mesh->LocalTransform.SetTranslate(-center);
    mTrnNode->SetChild(0, mesh);

    for (i = 2; i < mLimitedQuantity + 2; ++i)
    {
        mTrnNode->SetChild(i, CreateSphere());
    }

    TriMesh* sphere = StaticCast<TriMesh>(mTrnNode->GetChild(1));
    sphere->LocalTransform.SetTranslate(
        mVertices[mLimitedQuantity - 1] - center);

    // Update the scene, center-and-fit to frustum.
    mScene->Update();
    mTrnNode->LocalTransform.SetTranslate(-mScene->WorldBound.GetCenter());
    APoint camPosition = APoint::ORIGIN -
        2.5f*mScene->WorldBound.GetRadius()*mCamera->GetDVector();
    mCamera->SetPosition(camPosition);

    mCuller.ComputeVisibleSet(mScene);
}
//----------------------------------------------------------------------------
void ConvexHull3D::RegenerateHull ()
{
    delete0(mHull);
    mHull = new0 ConvexHull3f(mLimitedQuantity, mVertices, 0.001f, false,
        mQueryType);

    if (mHull->GetDimension() == 2)
    {
        ConvexHull3f* save = (ConvexHull3f*)mHull;
        mHull = save->GetConvexHull2();
        delete0(save);
    }
}
//----------------------------------------------------------------------------
TriMesh* ConvexHull3D::CreateSphere ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);

    Float3 white(1.0f, 1.0f, 1.0f);
    float radius = 0.01f;
    TriMesh* sphere = StandardMesh(vformat).Sphere(8, 8, radius);
    VertexBufferAccessor vba(sphere);
    for (int i = 0; i < vba.GetNumVertices(); ++i)
    {
        vba.Color<Float3>(0, i) = white;
    }

    sphere->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());
    return sphere;
}
//----------------------------------------------------------------------------
