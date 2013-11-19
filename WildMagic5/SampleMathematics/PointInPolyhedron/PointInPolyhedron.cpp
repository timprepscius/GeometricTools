// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "PointInPolyhedron.h"

WM5_WINDOW_APPLICATION(PointInPolyhedron);

// Enable only one at a time to test the algorithm.
//#define TRIFACES
//#define CVXFACES0
//#define CVXFACES1
//#define CVXFACES2
//#define SIMFACES0
#define SIMFACES1

//----------------------------------------------------------------------------
PointInPolyhedron::PointInPolyhedron ()
    :
    WindowApplication3("SampleMathematics/PointInPolyhedron", 0, 0, 1024,
        768, Float4(0.75f, 0.75f, 0.75f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
    mQuery = 0;
    mTFaces = 0;
    mCFaces = 0;
    mSFaces = 0;

    mNumRays = 5;
    mRayDirections = new1<Vector3f>(mNumRays);
    for (int i = 0; i < mNumRays; ++i)
    {
        double point[3];
        RandomPointOnHypersphere(3, point);
        for (int j = 0; j < 3; ++j)
        {
            mRayDirections[i][j] = (float)point[j];
        }
    }
}
//----------------------------------------------------------------------------
bool PointInPolyhedron::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 0.001f, 10.0f);
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

    InitializeCameraMotion(0.01f, 0.01f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void PointInPolyhedron::OnTerminate ()
{
    delete1(mRayDirections);

    mScene = 0;
    mWireState = 0;
    mPoints = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void PointInPolyhedron::OnIdle ()
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
bool PointInPolyhedron::OnKeyDown (unsigned char key, int x, int y)
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
        break;
    }

    return false;
}
//----------------------------------------------------------------------------
void PointInPolyhedron::CreateScene ()
{
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    // Create a semitransparent sphere mesh.
    VertexFormat* vformatMesh = VertexFormat::Create(1,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0);
    TriMesh* mesh = StandardMesh(vformatMesh).Sphere(16, 16, 1.0f);
    Material* material = new0 Material();
    material->Diffuse = Float4(1.0f, 0.0f, 0.0f, 0.25f);
    VisualEffectInstance* instance = MaterialEffect::CreateUniqueInstance(
        material);
    instance->GetEffect()->GetAlphaState(0, 0)->BlendEnabled = true;
    mesh->SetEffectInstance(instance);

    // Create the data structures for the polyhedron that represents the
    // sphere mesh.
    CreateQuery(mesh);

    // Create a set of random points.  Points inside the polyhedron are
    // colored white.  Points outside the polyhedron are colored blue.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(1024, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);
    Float3 white(1.0f, 1.0f, 1.0f);
    Float3 blue(0.0f, 0.0f, 1.0f);
    for (int i = 0; i < vba.GetNumVertices(); ++i)
    {
        Vector3f random(Mathf::SymmetricRandom(),
            Mathf::SymmetricRandom(), Mathf::SymmetricRandom());

        vba.Position<Vector3f>(i) = random;

        if (mQuery->Contains(random))
        {
            vba.Color<Float3>(0, i) = white;
        }
        else
        {
            vba.Color<Float3>(0, i) = blue;
        }
    }

    DeleteQuery();

    mPoints = new0 Polypoint(vformat, vbuffer);
    mPoints->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());

    mScene->AttachChild(mPoints);
    mScene->AttachChild(mesh);
}
//----------------------------------------------------------------------------
void PointInPolyhedron::CreateQuery (TriMesh* mesh)
{
    const VertexBuffer* vbuffer = mesh->GetVertexBuffer();
    const int numVertices = vbuffer->GetNumElements();
    const Vector3f* vertices = (Vector3f*)vbuffer->GetData();

    const IndexBuffer* ibuffer = mesh->GetIndexBuffer();
    const int numIndices = ibuffer->GetNumElements();
    const int numFaces = numIndices/3;
    const int* indices = (int*)ibuffer->GetData();
    const int* currentIndex = indices;
    int i;

#ifdef TRIFACES
    mTFaces = new1<PointInPolyhedron3f::TriangleFace>(numFaces);

    for (i = 0; i < numFaces; ++i)
    {
        int v0 = *currentIndex++;
        int v1 = *currentIndex++;
        int v2 = *currentIndex++;

        mTFaces[i].Indices[0] = v0;
        mTFaces[i].Indices[1] = v1;
        mTFaces[i].Indices[2] = v2;
        mTFaces[i].Plane = Plane3f(vertices[v0], vertices[v1], vertices[v2]);
    }

    mQuery = new0 PointInPolyhedron3f(numVertices, vertices, numFaces,
        mTFaces, mNumRays, mRayDirections);
#endif

#if defined(CVXFACES0) || defined(CVXFACES1) || defined(CVXFACES2)
    mCFaces = new1<PointInPolyhedron3f::ConvexFace>(numFaces);

    for (i = 0; i < numFaces; ++i)
    {
        int v0 = *currentIndex++;
        int v1 = *currentIndex++;
        int v2 = *currentIndex++;

        mCFaces[i].Indices.resize(3);
        mCFaces[i].Indices[0] = v0;
        mCFaces[i].Indices[1] = v1;
        mCFaces[i].Indices[2] = v2;
        mCFaces[i].Plane = Plane3f(vertices[v0], vertices[v1], vertices[v2]);
    }

#ifdef CVXFACES0
    mQuery = new0 PointInPolyhedron3f(numVertices, vertices, numFaces,
        mCFaces, mNumRays, mRayDirections, 0);
#else
#ifdef CVXFACES1
    mQuery = new0 PointInPolyhedron3f(numVertices, vertices, numFaces,
        mCFaces, mNumRays, mRayDirections, 1);
#else
    mQuery = new0 PointInPolyhedron3f(numVertices, vertices, numFaces,
        mCFaces, mNumRays, mRayDirections, 2);
#endif
#endif
#endif

#if defined (SIMFACES0) || defined (SIMFACES1)
    mSFaces = new1<PointInPolyhedron3f::SimpleFace>(numFaces);

    for (i = 0; i < numFaces; ++i)
    {
        int v0 = *currentIndex++;
        int v1 = *currentIndex++;
        int v2 = *currentIndex++;

        mSFaces[i].Indices.resize(3);
        mSFaces[i].Indices[0] = v0;
        mSFaces[i].Indices[1] = v1;
        mSFaces[i].Indices[2] = v2;
        mSFaces[i].Plane = Plane3f(vertices[v0], vertices[v1], vertices[v2]);

        mSFaces[i].Triangles.resize(3);
        mSFaces[i].Triangles[0] = v0;
        mSFaces[i].Triangles[1] = v1;
        mSFaces[i].Triangles[2] = v2;
    }

#ifdef SIMFACES0
    mQuery = new0 PointInPolyhedron3f(numVertices, vertices, numFaces,
        mSFaces, mNumRays, mRayDirections, 0);
#else
    mQuery = new0 PointInPolyhedron3f(numVertices, vertices, numFaces,
        mSFaces, mNumRays, mRayDirections, 1);
#endif
#endif
}
//----------------------------------------------------------------------------
void PointInPolyhedron::DeleteQuery ()
{
    delete0(mQuery);

#ifdef TRIANGLE_FACES
    delete1(mTFaces);
#endif
#if defined(CVXFACES0) || defined(CVXFACES1) || defined(CVXFACES2)
    delete1(mCFaces);
#endif
#if defined (SIMFACES0) || defined (SIMFACES1)
    delete1(mSFaces);
#endif
}
//----------------------------------------------------------------------------
