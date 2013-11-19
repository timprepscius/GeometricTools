// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Delaunay3D.h"

WM5_WINDOW_APPLICATION(Delaunay3D);

//----------------------------------------------------------------------------
Delaunay3D::Delaunay3D ()
    :
    WindowApplication3("SampleMathematics/Delaunay3D", 0, 0, 640, 480,
        Float4(1.0f, 1.0f, 1.0f, 1.0f))
{
    mDelaunay = 0;
}
//----------------------------------------------------------------------------
bool Delaunay3D::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 0.1f, 100.0f);
    APoint camPosition(0.0f, 0.0f, -4.0f);
    AVector camDVector(0.0f, 0.0f, 1.0f);
    AVector camUVector(0.0f, 1.0f, 0.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    CreateScene();

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.1f, 0.01f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void Delaunay3D::OnTerminate ()
{
    delete0(mDelaunay);

    mScene = 0;
    mCullState = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void Delaunay3D::OnIdle ()
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
        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool Delaunay3D::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication3::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
    case 's':
    case 'S':
        DoSearch();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void Delaunay3D::CreateScene ()
{
    int i, numVertices;
    Vector3f* vertices;
#if 0
    // test a cube
    numVertices = 8;
    vertices = new1<Vector3f>(numVertices);
    vertices[0] = Vector3f(0.0f, 0.0f, 0.0f);
    vertices[1] = Vector3f(1.0f, 0.0f, 0.0f);
    vertices[2] = Vector3f(1.0f, 1.0f, 0.0f);
    vertices[3] = Vector3f(0.0f, 1.0f, 0.0f);
    vertices[4] = Vector3f(0.0f, 0.0f, 1.0f);
    vertices[5] = Vector3f(1.0f, 0.0f, 1.0f);
    vertices[6] = Vector3f(1.0f, 1.0f, 1.0f);
    vertices[7] = Vector3f(0.0f, 1.0f, 1.0f);
#endif
#if 1
    // randomly generated points
    numVertices = 128;
    vertices = new1<Vector3f>(numVertices);
    vertices[0] = Vector3f::ZERO;
    for (i = 1; i < numVertices; ++i)
    {
        vertices[i].X() = Mathf::SymmetricRandom();
        vertices[i].Y() = Mathf::SymmetricRandom();
        vertices[i].Z() = Mathf::SymmetricRandom();
    }
#endif

    mMin = vertices[0];
    mMax = vertices[0];
    for (i = 1; i < numVertices; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            float value = vertices[i][j];
            if (value < mMin[j])
            {
                mMin[j] = value;
            }
            else if (value > mMax[j])
            {
                mMax[j] = value;
            }
        }
    }

    mDelaunay = new0 Delaunay3f(numVertices, vertices, 0.001f, true,
        Query::QT_REAL);

    mScene = new0 Node();
    mCullState = new0 CullState();
    mCullState->Enabled = false;
    mRenderer->SetOverrideCullState(mCullState);

    mScene->AttachChild(CreateSphere());
    for (int j = 0; j < mDelaunay->GetNumSimplices(); ++j)
    {
        mScene->AttachChild(CreateTetra(j));
    }
}
//----------------------------------------------------------------------------
TriMesh* Delaunay3D::CreateSphere () const
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);

    TriMesh* mesh = StandardMesh(vformat).Sphere(8, 8, 0.01f);
    mesh->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());
    mesh->Culling = Spatial::CULL_ALWAYS;
    return mesh;
}
//----------------------------------------------------------------------------
TriMesh* Delaunay3D::CreateTetra (int index) const
{
    const Vector3f* dvertices = mDelaunay->GetVertices();
    const int* dindices = mDelaunay->GetIndices();

    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT4, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(4, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);
    vba.Position<Vector3f>(0) = dvertices[dindices[4*index    ]];
    vba.Position<Vector3f>(1) = dvertices[dindices[4*index + 1]];
    vba.Position<Vector3f>(2) = dvertices[dindices[4*index + 2]];
    vba.Position<Vector3f>(3) = dvertices[dindices[4*index + 3]];
    Float4 lightGray(0.75f, 0.75f, 0.75f, 1.0f);
    vba.Color<Float4>(0, 0) = lightGray;
    vba.Color<Float4>(0, 1) = lightGray;
    vba.Color<Float4>(0, 2) = lightGray;
    vba.Color<Float4>(0, 3) = lightGray;

    IndexBuffer* ibuffer = new0 IndexBuffer(12, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[ 0] = 0;  indices[ 1] = 1;  indices[ 2] = 2;
    indices[ 3] = 0;  indices[ 4] = 3;  indices[ 5] = 1;
    indices[ 6] = 0;  indices[ 7] = 2;  indices[ 8] = 3;
    indices[ 9] = 3;  indices[10] = 2;  indices[11] = 1;

    TriMesh* tetra = new0 TriMesh(vformat, vbuffer, ibuffer);
    VisualEffectInstance* instance =
        VertexColor4Effect::CreateUniqueInstance();
    instance->GetEffect()->GetAlphaState(0, 0)->BlendEnabled = true;
    instance->GetEffect()->GetWireState(0, 0)->Enabled = true;
    tetra->SetEffectInstance(instance);

    return tetra;
}
//----------------------------------------------------------------------------
void Delaunay3D::ChangeTetraStatus (int index, const Float4& color,
    bool enableWire)
{
    Visual* tetra = DynamicCast<Visual>(mScene->GetChild(1 + index));
    assertion(tetra != 0, "Expecting a Visual object.\n");
    VertexBufferAccessor vba(tetra);
    for (int i = 0; i < 4; ++i)
    {
        vba.Color<Float4>(0, i) = color;
    }
    mRenderer->Update(tetra->GetVertexBuffer());

    VisualEffectInstance* instance = tetra->GetEffectInstance();
    instance->GetEffect()->GetWireState(0, 0)->Enabled = enableWire;
}
//----------------------------------------------------------------------------
void Delaunay3D::ChangeLastTetraStatus (int index, int vOpposite,
    const Float4& color, const Float4& oppositeColor)
{
    Visual* tetra = DynamicCast<Visual>(mScene->GetChild(1 + index));
    assertion(tetra != 0, "Expecting a Visual object.\n");
    VertexBufferAccessor vba(tetra);
    for (int i = 0; i < 4; ++i)
    {
        if (i != vOpposite)
        {
            vba.Color<Float4>(0, i) = color;
        }
        else
        {
            vba.Color<Float4>(0, i) = oppositeColor;
        }
    }
    mRenderer->Update(tetra->GetVertexBuffer());

    VisualEffectInstance* instance = tetra->GetEffectInstance();
    instance->GetEffect()->GetWireState(0, 0)->Enabled = false;
}
//----------------------------------------------------------------------------
void Delaunay3D::DoSearch ()
{
    // Make all tetra wireframe.
    const int numSimplices = mDelaunay->GetNumSimplices();
    Float4 lightGray(0.75f, 0.75f, 0.75f, 1.0f);
    int i;
    for (i = 0; i < numSimplices; ++i)
    {
        ChangeTetraStatus(i, lightGray, true);
    }

    // Generate random point in AABB of data set.
    Vector3f random;
    random.X() = Mathf::IntervalRandom(mMin.X(), mMax.X());
    random.Y() = Mathf::IntervalRandom(mMin.Y(), mMax.Y());
    random.Z() = Mathf::IntervalRandom(mMin.Z(), mMax.Z());

    // Move sphere to this location.
    Spatial* sphere = mScene->GetChild(0);
    sphere->Culling = Spatial::CULL_DYNAMIC;
    sphere->LocalTransform.SetTranslate(random);
    sphere->Update();

    if (mDelaunay->GetContainingTetrahedron(random) >= 0)
    {
        // Make all tetra on the path solid.
        const int pathLast = mDelaunay->GetPathLast();
        for (i = 0; i <= pathLast; ++i)
        {
            int index = mDelaunay->GetPath()[i];
            float red, blue;
            if (pathLast > 0)
            {
                red = i/(float)pathLast;
                blue = 1.0f - red;
            }
            else
            {
                red = 1.0f;
                blue = 0.0f;
            }
            ChangeTetraStatus(index, Float4(red, 0.0f, blue, 0.5f), false);
        }
    }
    else
    {
        // The point is outside the convex hull.  Change the wireframe
        // color for the last visited face in the search path.
        int index = mDelaunay->GetPath()[mDelaunay->GetPathLast()];
        int v0, v1, v2, v3;
        int vOpposite = mDelaunay->GetLastFace(v0, v1, v2, v3);
        ChangeLastTetraStatus(index, vOpposite,
            Float4(0.0f, 1.0f, 0.0f, 0.5f),
            Float4(0.0f, 0.25f, 0.0f, 0.5f));
    }

    mCuller.ComputeVisibleSet(mScene);
}
//----------------------------------------------------------------------------
