// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "IntersectConvexPolyhedra.h"

WM5_WINDOW_APPLICATION(IntersectConvexPolyhedra);

//----------------------------------------------------------------------------
IntersectConvexPolyhedra::IntersectConvexPolyhedra ()
    :
    WindowApplication3("SampleMathematics/IntersectConvexPolyhedra", 0, 0,
        640, 480, Float4(0.75f, 0.75f, 0.75f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
}
//----------------------------------------------------------------------------
bool IntersectConvexPolyhedra::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 0.1f, 1000.0f);
    APoint camPosition(16.0f, 0.0f, 0.0f);
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
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void IntersectConvexPolyhedra::OnTerminate ()
{
    mScene = 0;
    mMeshPoly0 = 0;
    mMeshPoly1 = 0;
    mMeshIntersection = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void IntersectConvexPolyhedra::OnIdle ()
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
bool IntersectConvexPolyhedra::OnKeyDown (unsigned char key, int x, int y)
{
    switch (key)
    {
    case 's':
    case 'S':
        InitializeObjectMotion(mScene);
        return true;
    case '0':
        InitializeObjectMotion(mMeshPoly0);
        return true;
    case '1':
        InitializeObjectMotion(mMeshPoly1);
        return true;
    case 'r':
    case 'R':
        ComputeIntersection();
        mScene->Update();
        mCuller.ComputeVisibleSet(mScene);
        return true;
    }

    return WindowApplication3::OnKeyDown(key, x, y);
}
//----------------------------------------------------------------------------
void IntersectConvexPolyhedra::CreateScene ()
{
    mScene = new0 Node();
    mMotionObject = mScene;

    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    // Attach a dummy intersection mesh.  If the intersection is nonempty,
    // the Culling flag will be modified to CULL_DYNAMIC.  The intersection
    // is drawn as a solid.
    mMeshIntersection = StandardMesh(vformat).Tetrahedron();
    VertexBufferAccessor vba(mMeshIntersection);
    Float3 green(0.0f, 1.0f, 0.0f);
    int i, j;
    for (i = 0; i < vba.GetNumVertices(); ++i)
    {
        vba.Color<Float3>(0, i) = green;
    }
    mMeshIntersection->SetEffectInstance(
        VertexColor3Effect::CreateUniqueInstance());
    mMeshIntersection->Culling = Spatial::CULL_ALWAYS;
    mScene->AttachChild(mMeshIntersection);

    // The first polyhedron is an ellipsoid.
    ConvexPolyhedronf::CreateEggShape(Vector3f::ZERO, 1.0f, 1.0f, 2.0f, 2.0f,
        4.0f, 4.0f, 3, mWorldPoly0);

    // Build the corresponding mesh.
    int numVertices = mWorldPoly0.GetNumVertices();
    int numTriangles = mWorldPoly0.GetNumTriangles();
    int numIndices = 3*numTriangles;
    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, vstride);
    IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, sizeof(int));
    Float3 red(1.0f, 0.0f, 0.0f);
    vba.ApplyTo(vformat, vbuffer);
    for (i = 0; i < numVertices; ++i)
    {
        vba.Position<Vector3f>(i) = mWorldPoly0.Point(i);
        vba.Color<Float3>(0,i) = red;
    }
    int* indices = (int*)ibuffer->GetData();
    for (i = 0; i < numTriangles; ++i)
    {
        const MTTriangle& triangle = mWorldPoly0.GetTriangle(i);
        for (j = 0; j < 3; ++j)
        {
            indices[3*i + j] = mWorldPoly0.GetVLabel(triangle.GetVertex(j));
        }
    }

    mMeshPoly0 = new0 TriMesh(vformat, vbuffer, ibuffer);
    VisualEffectInstance* instance =
        VertexColor3Effect::CreateUniqueInstance();
    instance->GetEffect()->GetWireState(0, 0)->Enabled = true;
    mMeshPoly0->SetEffectInstance(instance);
    mMeshPoly0->LocalTransform.SetTranslate(APoint(0.0f, 2.0f, 0.0f));
    mScene->AttachChild(mMeshPoly0);

    // The second polyhedron is egg shaped.
    ConvexPolyhedronf::CreateEggShape(Vector3f::ZERO, 2.0f, 2.0f, 4.0f, 4.0f,
        5.0f, 3.0f, 4, mWorldPoly1);

    // Build the corresponding mesh.
    numVertices = mWorldPoly1.GetNumVertices();
    numTriangles = mWorldPoly1.GetNumTriangles();
    numIndices = 3*numTriangles;
    vbuffer = new0 VertexBuffer(numVertices, vstride);
    ibuffer = new0 IndexBuffer(numIndices, sizeof(int));
    Float3 blue(0.0f, 0.0f, 1.0f);
    vba.ApplyTo(vformat, vbuffer);
    for (i = 0; i < numVertices; ++i)
    {
        vba.Position<Vector3f>(i) = mWorldPoly1.Point(i);
        vba.Color<Float3>(0, i) = blue;
    }
    indices = (int*)ibuffer->GetData();
    for (i = 0; i < numTriangles; ++i)
    {
        const MTTriangle& triangle = mWorldPoly1.GetTriangle(i);
        for (j = 0; j < 3; ++j)
        {
            indices[3*i + j] = mWorldPoly1.GetVLabel(triangle.GetVertex(j));
        }
    }

    mMeshPoly1 = new0 TriMesh(vformat, vbuffer, ibuffer);
    instance = VertexColor3Effect::CreateUniqueInstance();
    instance->GetEffect()->GetWireState(0, 0)->Enabled = true;
    mMeshPoly1->SetEffectInstance(instance);
    mMeshPoly1->LocalTransform.SetTranslate(APoint(0.0f, -2.0f, 0.0f));
    mScene->AttachChild(mMeshPoly1);

    ComputeIntersection();
}
//----------------------------------------------------------------------------
void IntersectConvexPolyhedra::ComputeIntersection ()
{
    // Transform the model-space vertices to world space.
    VertexBufferAccessor vba(mMeshPoly0);
    int i;
    for (i = 0; i < vba.GetNumVertices(); ++i)
    {
        APoint modPos = vba.Position<Float3>(i);
        APoint locPos = mMeshPoly0->LocalTransform*modPos;
        mWorldPoly0.Point(i) = Vector3f(locPos[0], locPos[1], locPos[2]);
    }
    mWorldPoly0.UpdatePlanes();

    vba.ApplyTo(mMeshPoly1);
    for (i = 0; i < vba.GetNumVertices(); ++i)
    {
        APoint modPos = vba.Position<Float3>(i);
        APoint locPos = mMeshPoly1->LocalTransform*modPos;
        mWorldPoly1.Point(i) = Vector3f(locPos[0], locPos[1], locPos[2]);
    }
    mWorldPoly1.UpdatePlanes();

    // Compute the intersection (if any) in world space.
    bool hasIntersection = mWorldPoly0.FindIntersection(mWorldPoly1,
        mIntersection);

    if (hasIntersection)
    {
        // Build the corresponding mesh.
        int numVertices = mIntersection.GetNumVertices();
        int numTriangles = mIntersection.GetNumTriangles();
        int numIndices = 3*numTriangles;
        VertexFormat* vformat = mMeshPoly0->GetVertexFormat();
        int vstride = vformat->GetStride();
        VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, vstride);
        IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, sizeof(int));
        Float3 green(0.0f, 1.0f, 0.0f);
        vba.ApplyTo(vformat, vbuffer);
        for (i = 0; i < numVertices; ++i)
        {
            vba.Position<Vector3f>(i) = mIntersection.Point(i);
            vba.Color<Float3>(0, i) = green;
        }
        int* indices = (int*)ibuffer->GetData();
        for (i = 0; i < numTriangles; ++i)
        {
            const MTTriangle& triangle = mIntersection.GetTriangle(i);
            for (int j = 0; j < 3; ++j)
            {
                indices[3*i + j] =
                    mIntersection.GetVLabel(triangle.GetVertex(j));
            }
        }

        mMeshIntersection = new0 TriMesh(vformat, vbuffer, ibuffer);
        mMeshIntersection->SetEffectInstance(
            VertexColor3Effect::CreateUniqueInstance());
        mScene->SetChild(0, mMeshIntersection);

        mMeshIntersection->Culling = Spatial::CULL_DYNAMIC;
    }
    else
    {
        mMeshIntersection->Culling = Spatial::CULL_ALWAYS;
    }
}
//----------------------------------------------------------------------------
