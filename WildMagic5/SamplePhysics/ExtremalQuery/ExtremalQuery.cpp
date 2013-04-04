// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "ExtremalQuery.h"
#include "Wm5ExtremalQuery3BSP.h"
#include "Wm5ExtremalQuery3PRJ.h"

WM5_WINDOW_APPLICATION(ExtremalQuery);

//----------------------------------------------------------------------------
ExtremalQuery::ExtremalQuery ()
    :
    WindowApplication3("SamplePhysics/ExtremalQuery", 0, 0, 640, 480,
        Float4(0.8f, 0.8f, 0.8f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
    mConvexPolyhedron = 0;
    mExtremalQuery = 0;
}
//----------------------------------------------------------------------------
bool ExtremalQuery::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up an orthogonal camera.  This projection type is used to make it
    // clear that the displayed extreme points really are extreme!  (The
    // perspective projection is deceptive.)
    mCamera = new0 Camera(false);
    mRenderer->SetCamera(mCamera);
    mCamera->SetFrustum(1.0f, 1000.0f, -1.5f, 1.5f, -2.0, 2.0f);
    APoint camPosition(4.0f, 0.0f, 0.0f);
    AVector camDVector(-1.0f, 0.0f, 0.0f);
    AVector camUVector(0.0f, 0.0f, 1.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    // Set up the scene.
    CreateScene();

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void ExtremalQuery::OnTerminate ()
{
    delete0(mConvexPolyhedron);
    delete0(mExtremalQuery);

    mScene = 0;
    mWireState = 0;
    mCullState = 0;
    mMaxSphere = 0;
    mMinSphere = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void ExtremalQuery::OnIdle ()
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
bool ExtremalQuery::OnKeyDown (unsigned char key, int x, int y)
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
bool ExtremalQuery::OnMotion (int button, int x, int y,
    unsigned int modifiers)
{
    WindowApplication3::OnMotion(button, x, y, modifiers);
    UpdateExtremePoints();
    return true;
}
//----------------------------------------------------------------------------
void ExtremalQuery::CreateScene ()
{
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);
    mCullState = new0 CullState();
    mCullState->Enabled = false;
    mRenderer->SetOverrideCullState(mCullState);

    const int numVertices = 32;
    CreateConvexPolyhedron(numVertices);
    mScene->AttachChild(CreateVisualConvexPolyhedron());

    // Use small spheres to show the extreme points in the camera's right
    // direction.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);

    StandardMesh sm(vformat);
    VertexColor3Effect* effect = new0 VertexColor3Effect();

    // maximum sphere
    mMaxSphere = sm.Sphere(8, 8, 0.05f);
    mMaxSphere->SetEffectInstance(effect->CreateInstance());
    mScene->AttachChild(mMaxSphere);

    // minimum sphere
    mMinSphere = sm.Sphere(8, 8, 0.05f);
    mMinSphere->SetEffectInstance(effect->CreateInstance());
    mScene->AttachChild(mMinSphere);

    UpdateExtremePoints();
}
//----------------------------------------------------------------------------
void ExtremalQuery::CreateConvexPolyhedron (int numVertices)
{
    // Create the convex hull of a randomly generated set of points on the
    // unit sphere.
    Vector3f* vertices = new1<Vector3f>(numVertices);
    int i, j;
    for (i = 0; i < numVertices; ++i)
    {
        for (j = 0; j < 3; ++j)
        {
            vertices[i][j] = Mathf::SymmetricRandom();
        }
        vertices[i].Normalize();
    }

    ConvexHull3f hull(numVertices, vertices, 0.001f, false, Query::QT_INT64);
    assertion(hull.GetDimension() == 3, "Invalid polyhedron.\n");
    int numIndices = 3*hull.GetNumSimplices();
    int* indices = new1<int>(numIndices);
    memcpy(indices, hull.GetIndices(), numIndices*sizeof(int));

    mConvexPolyhedron = new0 ConvexPolyhedron3f(numVertices, vertices,
        numIndices/3, indices, 0);

#ifdef USE_BSP_QUERY
    mExtremalQuery = new0 ExtremalQuery3BSPf(mConvexPolyhedron);
#else
    mExtremalQuery = new0 ExtremalQuery3PRJf(mConvexPolyhedron);
#endif

#ifdef MEASURE_TIMING_OF_QUERY
    // For timing purposes and determination of asymptotic order.
    const int imax = 10000000;
    Vector3f* directions = new1<Vector3f>(imax);
    for (i = 0; i < imax; ++i)
    {
        for (j = 0; j < 3; ++j)
        {
            directions[i][j] = Mathf::SymmetricRandom();
        }
        directions[i].Normalize();
    }

    clock_t start = clock();
    for (i = 0; i < imax; ++i)
    {
        int pos, neg;
        mExtremalQuery->GetExtremeVertices(directions[i], pos, neg);
    }
    clock_t final = clock();
    long diff = final - start;
    double time = ((double)diff)/(double)CLOCKS_PER_SEC;
    std::ofstream outFile("timing.txt");
    outFile << "time = " << time << " seconds" << std::endl;
    outFile.close();

    delete1(directions);
#endif
}
//----------------------------------------------------------------------------
Node* ExtremalQuery::CreateVisualConvexPolyhedron ()
{
    const Vector3f* vertices = mConvexPolyhedron->GetVertices();
    int numTriangles = mConvexPolyhedron->GetNumTriangles();
    int numIndices = 3*numTriangles;
    const int* polyIndices = mConvexPolyhedron->GetIndices();

    // Visualize the convex polyhedron as a collection of face-colored
    // triangles.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(numIndices, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);

    IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, sizeof(int));
    int* indices = (int*)ibuffer->GetData();

    int i;
    for (i = 0; i < numIndices; ++i)
    {
        vba.Position<Vector3f>(i) = vertices[polyIndices[i]];
        indices[i] = i;
    }

    TriMesh* mesh = new0 TriMesh(vformat, vbuffer, ibuffer);

    // Use randomly generated vertex colors.
    for (i = 0; i < numTriangles; ++i)
    {
        Float3 color;
        for (int j = 0; j < 3; ++j)
        {
            color[j] = Mathf::UnitRandom();
        }

        vba.Color<Float3>(0, 3*i  ) = color;
        vba.Color<Float3>(0, 3*i+1) = color;
        vba.Color<Float3>(0, 3*i+2) = color;
    }

    mesh->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());

    Node* root = new0 Node();
    root->AttachChild(mesh);
    return root;
}
//----------------------------------------------------------------------------
void ExtremalQuery::UpdateExtremePoints ()
{
    AVector rVector = mScene->WorldTransform.Inverse()*mCamera->GetRVector();
    Vector3f direction(rVector[0], rVector[1], rVector[2]);

    int posDir, negDir;
    mExtremalQuery->GetExtremeVertices(direction, posDir, negDir);

    Vector3f extreme = mConvexPolyhedron->GetVertex(posDir);
    mMaxSphere->LocalTransform.SetTranslate(extreme);

    extreme = mConvexPolyhedron->GetVertex(negDir);
    mMinSphere->LocalTransform.SetTranslate(extreme);

    mScene->Update();
}
//----------------------------------------------------------------------------
