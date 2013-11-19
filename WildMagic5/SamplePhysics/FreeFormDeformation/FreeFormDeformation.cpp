// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "FreeFormDeformation.h"

WM5_WINDOW_APPLICATION(FreeFormDeformation);

//----------------------------------------------------------------------------
FreeFormDeformation::FreeFormDeformation ()
    :
    WindowApplication3("SamplePhysics/FreeFormDeformation", 0, 0, 640, 480,
        Float4(0.5f, 0.0f, 1.0f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
    mQuantity = 4;
    mDegree = 3;
    mVolume = 0;
    mParameters = 0;
    mDoRandom = false;
    mAmplitude = 0.01f;
    mRadius = 0.25f;
    mLastUpdateTime = (float)GetTimeInSeconds();
    mMouseDown = false;
}
//----------------------------------------------------------------------------
bool FreeFormDeformation::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the scene graph.
    CreateScene();

    // Center-and-fit mesh for viewing by camera
    mMesh->Update();
    mTrnNode->LocalTransform.SetTranslate(-mScene->WorldBound.GetCenter());
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 0.1f, 100.0f);
    AVector camDVector(0.0f, 0.0f, 1.0f);
    AVector camUVector(0.0f, 1.0f, 0.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    APoint camPosition = APoint::ORIGIN -
        2.5f*mScene->WorldBound.GetRadius()*camDVector;
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.01f, 0.02f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void FreeFormDeformation::OnTerminate ()
{
    delete0(mVolume);
    delete1(mParameters);
    mPicker.Records.clear();

    mScene = 0;
    mTrnNode = 0;
    mWireState = 0;
    mMesh = 0;
    mPolysegmentRoot = 0;
    mControlRoot = 0;
    mSelected = 0;
    mControlActive = 0;
    mControlInactive = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void FreeFormDeformation::OnIdle ()
{
    MeasureTime();

    bool needsCulling = false;
    if (MoveCamera())
    {
        needsCulling = true;
    }

    if (MoveObject())
    {
        mScene->Update();
        needsCulling = true;
        mCuller.ComputeVisibleSet(mScene);
    }

    if (mDoRandom)
    {
        // Deform the mesh no faster than 30 frames per second.
        float time = (float)GetTimeInSeconds();
        if (time - mLastUpdateTime >= 0.0333333f)
        {
            mLastUpdateTime = time;
            DoRandomControlPoints();
            mScene->Update();
            needsCulling = true;
        }
    }

    if (needsCulling)
    {
        mCuller.ComputeVisibleSet(mScene);
    }

    // Draw the scene as fast as possible (not limited to 30 fps).
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
bool FreeFormDeformation::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication3::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
    case 'w':  // toggle wireframe
    case 'W':
        mWireState->Enabled = !mWireState->Enabled;
        return true;
    case 'a':  // toggle automated random versus user-adjusted controls
    case 'A':
        mDoRandom = !mDoRandom;
        return true;
    case 'c':  // toggle whether or not the polysegments/boxes are drawn
    case 'C':
        if (mPolysegmentRoot->Culling == Spatial::CULL_ALWAYS)
        {
            mPolysegmentRoot->Culling = Spatial::CULL_DYNAMIC;
        }
        else
        {
            mPolysegmentRoot->Culling = Spatial::CULL_ALWAYS;
        }

        if (mControlRoot->Culling == Spatial::CULL_ALWAYS)
        {
            mControlRoot->Culling = Spatial::CULL_DYNAMIC;
        }
        else
        {
            mControlRoot->Culling = Spatial::CULL_ALWAYS;
        }

        mCuller.ComputeVisibleSet(mScene);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool FreeFormDeformation::OnMouseClick (int button, int state, int x, int y,
    unsigned int)
{
    if (mDoRandom || button != MOUSE_LEFT_BUTTON)
    {
        return false;
    }

    if (state == MOUSE_DOWN)
    {
        mMouseDown = true;
        OnMouseDown(x, y);
    }
    else
    {
        mMouseDown = false;
    }

    return true;
}
//----------------------------------------------------------------------------
bool FreeFormDeformation::OnMotion (int, int x, int y, unsigned int)
{
    if (mDoRandom || !mMouseDown || !mSelected)
    {
        return false;
    }

    OnMouseMove(x, y);
    return true;
}
//----------------------------------------------------------------------------
void FreeFormDeformation::OnMouseDown (int x, int y)
{
    // Change to right-handed coordinates.
    y = GetHeight() - 1 - y;

    // The current selected control point is deactivated.
    if (mSelected)
    {
        mSelected->SetEffectInstance(mControlInactive);
        mSelected = 0;
    }

    // Determine which control point has been selected (if any).
    APoint origin;
    AVector direction;
    if (mRenderer->GetPickRay(x, y, origin, direction))
    {
        mPicker.Execute(mControlRoot, origin, direction, 0.0f,
            Mathf::MAX_REAL);

        if (mPicker.Records.size() > 0)
        {
            const PickRecord& record = mPicker.GetClosestNonnegative();
            TriMesh* box = DynamicCast<TriMesh>(record.Intersected);
            if (box)
            {
                APoint vertex[3];
                box->GetWorldTriangle(record.Triangle, vertex);
                mOldWorldPos =
                    vertex[0]*record.Bary[0] +
                    vertex[1]*record.Bary[1] +
                    vertex[2]*record.Bary[2];

                mSelected = box;
                mSelected->SetEffectInstance(mControlActive);
            }
        }
    }
}
//----------------------------------------------------------------------------
void FreeFormDeformation::OnMouseMove (int x, int y)
{
    // Change to right-handed coordinates.
    y = GetHeight() - 1 - y;

    // Construct a pick ray.  Te want to move the control point from its
    // current location to this ray.
    APoint origin;
    AVector direction;
    if (!mRenderer->GetPickRay(x, y, origin, direction))
    {
        return;
    }

    // Let E be the camera world origin, D be the camera world direction, and
    // U be the pick ray world direction.  Let C0 be the current location of
    // the picked point and let C1 be its desired new location.  We need to
    // choose t for which C1 = E + t*U.  Two possibilities are provided here,
    // both requiring computing: Diff = C0 - E.
    AVector diff = mOldWorldPos - mCamera->GetPosition();

    float t;
#if 0
    // The new world position is chosen to be at a distance along the pick
    // ray that is equal to the distance from the camera location to the old
    // world position.  That is, we require
    //   Length(C0-E) = Length(C1-E) = Length(t*U) = t.
    t = diff.Length();
#else
    // The new world position is in the same plane perpendicular to the
    // camera direction as the old world position is.  This plane is
    // Dot(D,X-C0) = 0, in which case we need
    //   0 = Dot(D,C1-C0) = Dot(D,E+t*U-C0) = Dot(D,E-C0) + t*Dot(D,U)
    // Solving for t, we have
    //   t = Dot(D,C0-E)/Dot(D,U)
    t = mCamera->GetDVector().Dot(diff)/mCamera->GetDVector().Dot(direction);
#endif
    APoint newWorldPos = origin + t*direction;

    // Move the control point to the new world location.  The technical
    // problem is that we need to modify the world coordinates for the
    // selected control point.  Thus, we need to determine how to change the
    // local translation in order to produce the correct world translation.
    Node* parent = (Node*)mSelected->GetParent();
    diff = newWorldPos - mOldWorldPos;
    AVector localDiff = parent->WorldTransform.Inverse()*diff;
    mSelected->LocalTransform.SetTranslate(
        mSelected->LocalTransform.GetTranslate() + localDiff);
    mSelected->Update();
    mOldWorldPos = newWorldPos;

    // Modify the control point itself.  It is known that the name string
    // has three single-digit numbers separated by blanks.
    const std::string& name = mSelected->GetName();
    int i0 = (int)(name[0] - '0');
    int i1 = (int)(name[2] - '0');
    int i2 = (int)(name[4] - '0');
    APoint trn = mSelected->LocalTransform.GetTranslate();
    mVolume->SetControlPoint(i0, i1, i2, Vector3f(trn[0], trn[1], trn[2]));

    // TO DO.  We need only update mesh vertices that are affected by the
    // change in one control point.  This requires working with the B-spline
    // basis function and knowing which (u,v,w) to evaluate at (i.e. determine
    // the local control region).
    UpdateMesh();

    // TO DO.  Only need to update neighboring lines.
    UpdatePolysegments();
}
//----------------------------------------------------------------------------
void FreeFormDeformation::CreateScene ()
{
    mScene = new0 Node();
    mTrnNode = new0 Node();
    mScene->AttachChild(mTrnNode);
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    // Load the small brain data set.  It has positions (x,y,z) and texture
    // coordinates (u,v).
    std::string path = Environment::GetPathR("SmallBrainPT2.wmvf");
    Visual::PrimitiveType type;
    VertexFormat* vformat;
    VertexBuffer* vbuffer;
    IndexBuffer* ibuffer;
    Visual::LoadWMVF(path, type, vformat, vbuffer, ibuffer);

    path = Environment::GetPathR("Quartz.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    mMesh = new0 TriMesh(vformat, vbuffer, ibuffer);
    mMesh->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(texture,
        Shader::SF_LINEAR, Shader::SC_CLAMP_EDGE, Shader::SC_CLAMP_EDGE));

    mTrnNode->AttachChild(mMesh);

    CreateBSplineVolume();
    CreatePolylines();
    CreateControlBoxes();
}
//----------------------------------------------------------------------------
void FreeFormDeformation::CreateBSplineVolume ()
{
    // Generate the B-spline volume function.
    mVolume = new0 BSplineVolumef(mQuantity, mQuantity, mQuantity, mDegree,
        mDegree, mDegree);

    // Get an AABB of the form [xmin,xmax]x[ymin,ymax]x[zmin,zmax].
    VertexBufferAccessor vba(mMesh);
    int numVertices = vba.GetNumVertices();
    Vector3f position = vba.Position<Vector3f>(0);
    mXMin = position[0];
    mYMin = position[1];
    mZMin = position[2];
    float xMax = mXMin, yMax = mYMin, zMax = mZMin;
    int i;
    for (i = 0; i < numVertices; ++i)
    {
        position = vba.Position<Vector3f>(i);

        if (position[0] < mXMin)
        {
            mXMin = position[0];
        }
        else if (position[0] > xMax)
        {
            xMax = position[0];
        }

        if (position[1] < mYMin)
        {
            mYMin = position[1];
        }
        else if (position[1] > yMax)
        {
            yMax = position[1];
        }

        if (position[2] < mZMin)
        {
            mZMin = position[2];
        }
        else if (position[2] > zMax)
        {
            zMax = position[2];
        }
    }

    // Generate the control points.
    float xRange = xMax - mXMin;
    float yRange = yMax - mYMin;
    float zRange = zMax - mZMin;
    mDX = xRange/(float)(mQuantity - 1);
    mDY = yRange/(float)(mQuantity - 1);
    mDZ = zRange/(float)(mQuantity - 1);
    Vector3f ctrl;
    for (int i0 = 0; i0 < mQuantity; ++i0)
    {
        ctrl[0] = mXMin + mDX*i0;
        for (int i1 = 0; i1 < mQuantity; ++i1)
        {
            ctrl[1] = mYMin + mDY*i1;
            for (int i2 = 0; i2 < mQuantity; i2++)
            {
                ctrl[2] = mZMin + mDZ*i2;
                mVolume->SetControlPoint(i0, i1, i2, ctrl);
            }
        }
    }

    // Compute the (u,v,w) values of the mesh relative to the B-spline volume.
    float invXRange = 1.0f/xRange;
    float invYRange = 1.0f/yRange;
    float invZRange = 1.0f/zRange;
    mParameters = new1<Vector3f>(numVertices);
    for (i = 0; i < numVertices; ++i)
    {
        const Vector3f& pos = vba.Position<Vector3f>(i);
        Vector3f& param = mParameters[i];
        param[0] = (pos[0] - mXMin)*invXRange;
        param[1] = (pos[1] - mYMin)*invYRange;
        param[2] = (pos[2] - mZMin)*invZRange;
    }
}
//----------------------------------------------------------------------------
void FreeFormDeformation::CreatePolylines ()
{
    // Generate the polylines that connect adjacent control points.
    mPolysegmentRoot = new0 Node();
    mTrnNode->AttachChild(mPolysegmentRoot);

    VertexColor3Effect* effect = new0 VertexColor3Effect();
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    VertexBufferAccessor vba;
    VertexBuffer* vbuffer;
    Polysegment* segment;

    int i0, i1, i2;
    for (i0 = 0; i0 < mQuantity; ++i0)
    {
        for (i1 = 0; i1 < mQuantity; ++i1)
        {
            for (i2 = 0; i2 < mQuantity-1; ++i2)
            {
                vbuffer = new0 VertexBuffer(2, vstride);
                vba.ApplyTo(vformat, vbuffer);
                vba.Position<Vector3f>(0) =
                    mVolume->GetControlPoint(i0, i1, i2);
                vba.Position<Vector3f>(1) =
                    mVolume->GetControlPoint(i0, i1, i2+1);
                vba.Color<Float3>(0, 0) = Float3(0.0f, 0.0f, 0.75f);
                vba.Color<Float3>(0, 1) = Float3(0.0f, 0.0f, 0.75f);
                segment = new0 Polysegment(vformat, vbuffer, true);
                segment->SetEffectInstance(effect->CreateInstance());
                mPolysegmentRoot->AttachChild(segment);
            }
        }

        for (i2 = 0; i2 < mQuantity; ++i2)
        {
            for (i1 = 0; i1 < mQuantity-1; ++i1)
            {
                vbuffer = new0 VertexBuffer(2, vstride);
                vba.ApplyTo(vformat, vbuffer);
                vba.Position<Vector3f>(0) =
                    mVolume->GetControlPoint(i0, i1, i2);
                vba.Position<Vector3f>(1) =
                    mVolume->GetControlPoint(i0, i1+1, i2);
                vba.Color<Float3>(0, 0) = Float3(0.0f, 0.75f, 0.0f);
                vba.Color<Float3>(0, 1) = Float3(0.0f, 0.75f, 0.0f);
                segment = new0 Polysegment(vformat, vbuffer, true);
                segment->SetEffectInstance(effect->CreateInstance());
                mPolysegmentRoot->AttachChild(segment);
            }
        }
    }

    for (i0 = 0; i0 < mQuantity-1; ++i0)
    {
        for (i1 = 0; i1 < mQuantity; ++i1)
        {
            for (i2 = 0; i2 < mQuantity; ++i2)
            {
                vbuffer = new0 VertexBuffer(2, vstride);
                vba.ApplyTo(vformat, vbuffer);
                vba.Position<Vector3f>(0) =
                    mVolume->GetControlPoint(i0, i1, i2);
                vba.Position<Vector3f>(1) =
                    mVolume->GetControlPoint(i0+1, i1, i2);
                vba.Color<Float3>(0,0) = Float3(0.75f, 0.0f, 0.0f);
                vba.Color<Float3>(0,1) = Float3(0.75f, 0.0f, 0.0f);
                segment = new0 Polysegment(vformat, vbuffer, true);
                segment->SetEffectInstance(effect->CreateInstance());
                mPolysegmentRoot->AttachChild(segment);
            }
        }
    }
}
//----------------------------------------------------------------------------
void FreeFormDeformation::CreateControlBoxes ()
{
    // Generate small boxes to represent the control points.
    mControlRoot = new0 Node();
    mTrnNode->AttachChild(mControlRoot);

    // Create a single box to be shared by each control point box.
    const float halfWidth = 0.02f;
    VertexFormat* vformat = VertexFormat::Create(1,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(8, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);
    vba.Position<Vector3f>(0) = Vector3f(-halfWidth, -halfWidth, -halfWidth);
    vba.Position<Vector3f>(1) = Vector3f(+halfWidth, -halfWidth, -halfWidth);
    vba.Position<Vector3f>(2) = Vector3f(+halfWidth, +halfWidth, -halfWidth);
    vba.Position<Vector3f>(3) = Vector3f(-halfWidth, +halfWidth, -halfWidth);
    vba.Position<Vector3f>(4) = Vector3f(-halfWidth, -halfWidth, +halfWidth);
    vba.Position<Vector3f>(5) = Vector3f(+halfWidth, -halfWidth, +halfWidth);
    vba.Position<Vector3f>(6) = Vector3f(+halfWidth, +halfWidth, +halfWidth);
    vba.Position<Vector3f>(7) = Vector3f(-halfWidth, +halfWidth, +halfWidth);

    IndexBuffer* ibuffer = new0 IndexBuffer(36, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[ 0] = 0;  indices[ 1] = 2;  indices[ 2] = 1;
    indices[ 3] = 0;  indices[ 4] = 3;  indices[ 5] = 2;
    indices[ 6] = 4;  indices[ 7] = 5;  indices[ 8] = 6;
    indices[ 9] = 4;  indices[10] = 6;  indices[11] = 7;
    indices[12] = 0;  indices[13] = 5;  indices[14] = 4;
    indices[15] = 0;  indices[16] = 1;  indices[17] = 5;
    indices[18] = 3;  indices[19] = 7;  indices[20] = 6;
    indices[21] = 3;  indices[22] = 6;  indices[23] = 2;
    indices[24] = 1;  indices[25] = 2;  indices[26] = 6;
    indices[27] = 1;  indices[28] = 6;  indices[29] = 5;
    indices[30] = 0;  indices[31] = 4;  indices[32] = 7;
    indices[33] = 0;  indices[34] = 7;  indices[35] = 3;

    // Create the materials and light to be attached to each box.
    Material* materialActive = new0 Material();
    materialActive->Emissive = Float4(0.0f, 0.0f, 0.0f, 1.0f);
    materialActive->Ambient = Float4(1.0f, 0.0f, 0.0f, 1.0f);
    materialActive->Diffuse = Float4(1.0f, 0.0f, 0.0f, 1.0f);
    materialActive->Specular = Float4(0.0f, 0.0f, 0.0f, 1.0f);

    Material* materialInactive = new0 Material();
    materialInactive->Emissive = Float4(0.0f, 0.0f, 0.0f, 1.0f);
    materialInactive->Ambient = Float4(0.75f, 0.75f, 0.75f, 1.0f);
    materialInactive->Diffuse = Float4(0.75f, 0.75f, 0.75f, 1.0f);
    materialInactive->Specular = Float4(0.0f, 0.0f, 0.0f, 1.0f);

    Light* light = new0 Light(Light::LT_AMBIENT);
    light->Ambient = Float4(1.0f, 1.0f, 1.0f, 1.0f);
    light->Diffuse = Float4(1.0f, 1.0f, 1.0f, 1.0f);
    light->Specular = Float4(0.0f, 0.0f, 0.0f, 1.0f);

    LightAmbEffect* effect = new0 LightAmbEffect();
    mControlActive = effect->CreateInstance(light, materialActive);
    mControlInactive = effect->CreateInstance(light, materialInactive);

    for (int i0 = 0; i0 < mQuantity; ++i0)
    {
        for (int i1 = 0; i1 < mQuantity; ++i1)
        {
            for (int i2 = 0; i2 < mQuantity; ++i2)
            {
                TriMesh* box = new0 TriMesh(vformat, vbuffer, ibuffer);
                Vector3f ctrl = mVolume->GetControlPoint(i0, i1, i2);
                box->LocalTransform.SetTranslate(ctrl);

                // Encode the indices in the name for later use.  This will
                // allow fast lookup of volume control points.
                char name[32];
                sprintf(name, "%d %d %d", i0, i1, i2);
                box->SetName(name);

                box->SetEffectInstance(mControlInactive);

                mControlRoot->AttachChild(box);
            }
        }
    }
}
//----------------------------------------------------------------------------
void FreeFormDeformation::UpdateMesh ()
{
    VertexBufferAccessor vba(mMesh);
    int numVertices = vba.GetNumVertices();
    for (int i = 0; i < numVertices; ++i)
    {
        const Vector3f& param = mParameters[i];
        Vector3f& position = vba.Position<Vector3f>(i);
        position = mVolume->GetPosition(param[0], param[1], param[2]);
    }
    mRenderer->Update(mMesh->GetVertexBuffer());
}
//----------------------------------------------------------------------------
void FreeFormDeformation::UpdatePolysegments ()
{
    VertexBufferAccessor vba;
    Polysegment* segment;

    int i0, i1, i2, i = 0;
    for (i0 = 0; i0 < mQuantity; ++i0)
    {
        for (i1 = 0; i1 < mQuantity; ++i1)
        {
            for (i2 = 0; i2 < mQuantity-1; ++i2)
            {
                segment = StaticCast<Polysegment>(
                    mPolysegmentRoot->GetChild(i));
                vba.ApplyTo(segment);
                vba.Position<Vector3f>(0) =
                    mVolume->GetControlPoint(i0, i1, i2);
                vba.Position<Vector3f>(1) =
                    mVolume->GetControlPoint(i0, i1, i2+1);
                mRenderer->Update(segment->GetVertexBuffer());
                ++i;
            }
        }

        for (i2 = 0; i2 < mQuantity; ++i2)
        {
            for (i1 = 0; i1 < mQuantity-1; ++i1)
            {
                segment = StaticCast<Polysegment>(
                    mPolysegmentRoot->GetChild(i));
                vba.ApplyTo(segment);
                vba.Position<Vector3f>(0) =
                    mVolume->GetControlPoint(i0, i1, i2);
                vba.Position<Vector3f>(1) =
                    mVolume->GetControlPoint(i0, i1+1, i2);
                mRenderer->Update(segment->GetVertexBuffer());
                ++i;
            }
        }
    }

    for (i0 = 0; i0 < mQuantity-1; ++i0)
    {
        for (i1 = 0; i1 < mQuantity; ++i1)
        {
            for (i2 = 0; i2 < mQuantity; ++i2)
            {
                segment = StaticCast<Polysegment>(
                    mPolysegmentRoot->GetChild(i));
                vba.ApplyTo(segment);
                vba.Position<Vector3f>(0) =
                    mVolume->GetControlPoint(i0, i1, i2);
                vba.Position<Vector3f>(1) =
                    mVolume->GetControlPoint(i0+1, i1, i2);
                mRenderer->Update(segment->GetVertexBuffer());
                ++i;
            }
        }
    }
}
//----------------------------------------------------------------------------
void FreeFormDeformation::UpdateControlBoxes ()
{
    for (int i0 = 0, i = 0; i0 < mQuantity; ++i0)
    {
        for (int i1 = 0; i1 < mQuantity; ++i1)
        {
            for (int i2 = 0; i2 < mQuantity; ++i2, ++i)
            {
                TriMesh* box = StaticCast<TriMesh>(mControlRoot->GetChild(i));
                Vector3f ctrl = mVolume->GetControlPoint(i0, i1, i2);
                box->LocalTransform.SetTranslate(ctrl);
            }
        }
    }
}
//----------------------------------------------------------------------------
void FreeFormDeformation::DoRandomControlPoints ()
{
    // Randomly perturb the control points, but stay near the original
    // control points.
    Vector3f ctrl;
    for (int i0 = 0; i0 < mQuantity; i0++)
    {
        ctrl[0] = mXMin + mDX*i0;
        for (int i1 = 0; i1 < mQuantity; i1++)
        {
            ctrl[1] = mYMin + mDY*i1;
            for (int i2 = 0; i2 < mQuantity; i2++)
            {
                ctrl[2] = mZMin + mDZ*i2;

                Vector3f newCtrl = mVolume->GetControlPoint(i0, i1, i2) +
                    mAmplitude*Vector3f(Mathf::SymmetricRandom(),
                    Mathf::SymmetricRandom(),Mathf::SymmetricRandom());

                Vector3f diff = newCtrl - ctrl;
                float length = diff.Length();
                if (length > mRadius)
                {
                    diff *= mRadius/length;
                }

                mVolume->SetControlPoint(i0, i1, i2, ctrl + diff);
            }
        }
    }

    UpdateMesh();
    UpdatePolysegments();
    UpdateControlBoxes();
}
//----------------------------------------------------------------------------
