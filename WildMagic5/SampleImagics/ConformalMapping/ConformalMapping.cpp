// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "ConformalMapping.h"

WM5_WINDOW_APPLICATION(ConformalMapping);

//----------------------------------------------------------------------------
ConformalMapping::ConformalMapping ()
    :
    WindowApplication3("SampleImagics/ConformalMapping", 0, 0, 640, 480,
        Float4(0.5f, 0.0f, 1.0f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
}
//----------------------------------------------------------------------------
bool ConformalMapping::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 0.1f, 100.0f);
    APoint camPosition(0.0f, 0.0f, -6.5f);
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

    InitializeCameraMotion(0.01f, 0.01f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void ConformalMapping::OnTerminate ()
{
    mScene = 0;
    mMeshTree = 0;
    mSphereTree = 0;
    mWireState = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void ConformalMapping::OnIdle ()
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
bool ConformalMapping::OnKeyDown (unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'w':
        mWireState->Enabled = !mWireState->Enabled;
        return true;
    case 'm':
        mMotionObject = mMeshTree;
        return true;
    case 's':
        mMotionObject = mSphereTree;
        return true;
    case 'r':
        mMotionObject = mScene;
        return true;
    }

    return WindowApplication3::OnKeyDown(key, x, y);
}
//----------------------------------------------------------------------------
void ConformalMapping::CreateScene ()
{
    // Load the brain mesh.  The mesh must have the topology of a sphere.
    std::string brainFile = Environment::GetPathR("BrainP.wmvf");
    Visual::PrimitiveType type;
    VertexFormat* vformat0;
    VertexBuffer* vbuffer0;
    IndexBuffer* ibuffer;
    Visual::LoadWMVF(brainFile, type, vformat0, vbuffer0, ibuffer);
    VertexBufferAccessor vba0(vformat0, vbuffer0);

    // The mesh has only positions.  Reallocate the vertex buffer to include
    // vertex colors.
    VertexFormat* vformat1 = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride1 = vformat1->GetStride();
    VertexBuffer* vbuffer1 = new0 VertexBuffer(vba0.GetNumVertices(),
        vstride1);
    VertexBufferAccessor vba1(vformat1, vbuffer1);
    for (int i = 0; i < vba0.GetNumVertices(); ++i)
    {
        vba1.Position<Float3>(i) = vba0.Position<Float3>(i);
    }

    TriMesh* mesh = new0 TriMesh(vformat1, vbuffer1, ibuffer);
    delete0(vformat0);
    delete0(vbuffer0);

    // Map the data to the cube [-10,10]^3.  This provides numerical
    // preconditioning for computing the conformal map.  The choice of 10 is
    // based on knowledge of the magnitude of the positions of vbuffer0.
    float extreme = 10.0f;
    ScaleToCube(extreme, mesh);

    // Use pseudocoloring based on mean curvature.
    PseudocolorVertices(mesh);

    // Compute the conformal map between the mesh and a sphere.
    TriMesh* sphere = DoMapping(mesh);

    // Use vertex coloring.
    VertexColor3Effect* effect = new0 VertexColor3Effect();
    mesh->SetEffectInstance(effect->CreateInstance());
    sphere->SetEffectInstance(effect->CreateInstance());

    // Create the root of the scene graph.
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    // Subtree for mesh.  This allows for a virtual trackball centered on
    // the mesh.
    mMeshTree = new0 Node();
    mMeshTree->LocalTransform.SetTranslate(APoint(2.0f, 0.0f, 0.0f));
    mMeshTree->LocalTransform.SetUniformScale(1.0f/extreme);
    mScene->AttachChild(mMeshTree);
    Node* meshParent = new0 Node();
    mMeshTree->AttachChild(meshParent);
    meshParent->AttachChild(mesh);
    meshParent->LocalTransform.SetTranslate(
        -mesh->GetModelBound().GetCenter());

    // Subtree for sphere.  This allows for a virtual trackball centered on
    // the sphere.
    mSphereTree = new0 Node();
    mSphereTree->LocalTransform.SetTranslate(APoint(-2.0f, 0.0f, 0.0f));
    mScene->AttachChild(mSphereTree);
    Node* sphereParent = new0 Node();
    mSphereTree->AttachChild(sphereParent);
    sphereParent->AttachChild(sphere);
    sphereParent->LocalTransform.SetTranslate(
        -sphere->GetModelBound().GetCenter());
}
//----------------------------------------------------------------------------
void ConformalMapping::ScaleToCube (float extreme, TriMesh* mesh)
{
    // Uniformly scale the cube to [-extreme,extreme]^3 for numerical
    // preconditioning for the conformal mapping.

    VertexBufferAccessor vba(mesh);
    float minValue = vba.Position<Float3>(0)[0], maxValue = minValue;
    int i, j;
    for (i = 0; i < vba.GetNumVertices(); ++i)
    {
        Float3 position = vba.Position<Float3>(i);
        for (j = 0; j < 3; ++j)
        {
            if (position[j] < minValue)
            {
                minValue = position[j];
            }
            else if (position[j] > maxValue)
            {
                maxValue = position[j];
            }
        }
    }
    float halfRange = 0.5f*(maxValue - minValue);
    float mult = extreme/halfRange;
    for (i = 0; i < vba.GetNumVertices(); ++i)
    {
        Float3& position = vba.Position<Float3>(i);
        for (j = 0; j < 3; ++j)
        {
            position[j] = -extreme + mult*(position[j] - minValue);
        }
    }

    mesh->UpdateModelSpace(Visual::GU_MODEL_BOUND_ONLY);
}
//----------------------------------------------------------------------------
void ConformalMapping::PseudocolorVertices (TriMesh* mesh)
{
    // Color the vertices according to mean curvature.

    VertexBufferAccessor vba(mesh);
    const int numVertices = vba.GetNumVertices();
    const int numTriangles = mesh->GetIndexBuffer()->GetNumElements()/3;
    const int* indices = (int*)mesh->GetIndexBuffer()->GetData();
    Vector3f* positions = new1<Vector3f>(numVertices);
    int i;
    for (i = 0; i < numVertices; ++i)
    {
        positions[i] = vba.Position<Vector3f>(i);
    }
    MeshCurvaturef mc(numVertices, positions, numTriangles, indices);
    delete1(positions);

    const float* minCurv = mc.GetMinCurvatures();
    const float* maxCurv = mc.GetMaxCurvatures();
    float minMeanCurvature = minCurv[0] + maxCurv[0];
    float maxMeanCurvature = minMeanCurvature;
    float* meanCurvatures = new1<float>(numVertices);
    for (i = 0; i < numVertices; ++i)
    {
        meanCurvatures[i] = minCurv[i] + maxCurv[i];
        if (meanCurvatures[i] < minMeanCurvature)
        {
            minMeanCurvature = meanCurvatures[i];
        }
        else if (meanCurvatures[i] > maxMeanCurvature)
        {
            maxMeanCurvature = meanCurvatures[i];
        }
    }

    for (i = 0; i < numVertices; ++i)
    {
        Float3& color = vba.Color<Float3>(0, i);
        if (meanCurvatures[i] > 0.0f)
        {
            color[0] = 0.5f*(1.0f + meanCurvatures[i]/maxMeanCurvature);
            color[1] = color[0];
            color[2] = 0.0f;
        }
        else if (meanCurvatures[i] < 0.0f)
        {
            color[0] = 0.0f;
            color[1] = 0.0f;
            color[2] = 0.5f*(1.0f - meanCurvatures[i]/minMeanCurvature);
        }
        else
        {
            color[0] = 0.0f;
            color[1] = 0.0f;
            color[2] = 0.0f;
        }
    }

    delete1(meanCurvatures);
}
//----------------------------------------------------------------------------
TriMesh* ConformalMapping::DoMapping (TriMesh* mesh)
{
    VertexBufferAccessor vba0(mesh);
    const int numVertices = vba0.GetNumVertices();
    IndexBuffer* ibuffer = mesh->GetIndexBuffer();
    const int numTriangles = ibuffer->GetNumElements()/3;
    const int* indices = (int*)ibuffer->GetData();
    Vector3f* positions = new1<Vector3f>(numVertices);
    int i;
    for (i = 0; i < numVertices; ++i)
    {
        positions[i] = vba0.Position<Vector3f>(i);
    }

    // Color the punctured triangle red.
    Float3 red(1.0f, 0.0f, 0.0f);
    vba0.Color<Float3>(0, indices[0]) = red;
    vba0.Color<Float3>(0, indices[1]) = red;
    vba0.Color<Float3>(0, indices[2]) = red;

    // Conformally map the mesh to plane, sphere, and cylinder.
    ConformalMapf cm(numVertices, positions, numTriangles, indices, 0);
    delete1(positions);
    const Vector3f* spherePositions = cm.GetSphereCoordinates();

    // Create a representation of the conformal sphere.
    VertexFormat* vformat = mesh->GetVertexFormat();
    int vstride = vformat->GetStride();
    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, vstride);
    VertexBufferAccessor vba1(vformat, vbuffer);
    for (i = 0; i < numVertices; ++i)
    {
        vba1.Position<Vector3f>(i) = spherePositions[i];
        vba1.Color<Float3>(0, i) = vba0.Color<Float3>(0, i);
    }

    TriMesh* sphere = new0 TriMesh(vformat, vbuffer, ibuffer);
    return sphere;
}
//----------------------------------------------------------------------------
