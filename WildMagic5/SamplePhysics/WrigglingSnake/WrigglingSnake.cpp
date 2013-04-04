// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "WrigglingSnake.h"

WM5_WINDOW_APPLICATION(WrigglingSnake);

float WrigglingSnake::msRadius = 0.0625f;

//#define SINGLE_STEP

//----------------------------------------------------------------------------
WrigglingSnake::WrigglingSnake ()
    :
    WindowApplication3("SamplePhysics/WrigglingSnake", 0, 0, 640, 480,
        Float4(1.0f, 0.823529f, 0.607843f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    mNumCtrlPoints = 32;
    mDegree = 3;
    mCenter = 0;
    mAmplitudes = new1<float>(mNumCtrlPoints);
    mPhases = new1<float>(mNumCtrlPoints);
    mNumShells = 4;
    mSlice = 0;
}
//----------------------------------------------------------------------------
WrigglingSnake::~WrigglingSnake ()
{
    delete1(mAmplitudes);
    delete1(mPhases);
}
//----------------------------------------------------------------------------
bool WrigglingSnake::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    CreateScene();

    // Center-and-fit for camera viewing.
    mScene->Update();
    mTrnNode->LocalTransform.SetTranslate(-mScene->WorldBound.GetCenter());
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 100.0f);
    AVector camDVector(0.0f, -1.0f, 0.0f);
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

    InitializeCameraMotion(0.01f, 0.001f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void WrigglingSnake::OnTerminate ()
{
    delete1(mSlice);

    mScene = 0;
    mTrnNode = 0;
    mSnakeRoot = 0;
    mSnakeBody = 0;
    mSnakeHead = 0;
    mWireState = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void WrigglingSnake::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    if (MoveObject())
    {
        mScene->Update();
    }

#ifndef SINGLE_STEP
    ModifyCurve();
#endif

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
bool WrigglingSnake::OnKeyDown (unsigned char key, int x, int y)
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
#ifdef SINGLE_STEP
    case 'g':
    case 'G':
        ModifyCurve();
        return true;
#endif
    }

    return false;
}
//----------------------------------------------------------------------------
float WrigglingSnake::Radial (float t)
{
    return msRadius*(2.0f*t)/(1.0f+t);
}
//----------------------------------------------------------------------------
void WrigglingSnake::CreateScene ()
{
    mScene = new0 Node();
    mTrnNode = new0 Node();
    mScene->AttachChild(mTrnNode);
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    CreateSnake();
}
//----------------------------------------------------------------------------
void WrigglingSnake::CreateSnake ()
{
    mSnakeRoot = new0 Node();
    mTrnNode->AttachChild(mSnakeRoot);
    CreateSnakeBody();
    CreateSnakeHead();
}
//----------------------------------------------------------------------------
void WrigglingSnake::CreateSnakeBody ()
{
    // Create the B-spline curve for the snake body.
    Vector3f* ctrlPoints = new1<Vector3f>(mNumCtrlPoints);
    int i;
    for (i = 0; i < mNumCtrlPoints; ++i)
    {
        // Control points for a snake.
        float ratio = ((float)i)/(float)(mNumCtrlPoints - 1);
        float x = -1.0f + 2.0f*ratio;
        float xMod = 10.0f*x - 4.0f;
        ctrlPoints[i].X() = x;
        ctrlPoints[i].Y() = msRadius*(1.5f + Mathf::ATan(xMod)/Mathf::PI);
        ctrlPoints[i].Z() = 0.0f;

        // Sinusoidal motion for snake.
        mAmplitudes[i] = 0.1f + ratio*Mathf::Exp(-ratio);
        mPhases[i] = 1.5f*ratio*Mathf::TWO_PI;
    }

    // The control points are copied by the curve objects.
    mCenter = new0 BSplineCurve3f(mNumCtrlPoints, ctrlPoints, mDegree, false,
        true);
    delete1(ctrlPoints);

    // Generate a tube surface.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    Float2 tcoordMin(0.0f, 0.0f), tcoordMax(1.0f, 16.0f);

    mSnakeBody = new0 TubeSurface(mCenter, Radial, false, Vector3f::UNIT_Y,
        128, 32, false, false, &tcoordMin, &tcoordMax, vformat);

    // Attach a texture to the snake body.
    std::string path = Environment::GetPathR("Snake.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    mSnakeBody->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(
        texture, Shader::SF_LINEAR_LINEAR, Shader::SC_REPEAT,
        Shader::SC_REPEAT));

    mSnakeRoot->AttachChild(mSnakeBody);
}
//----------------------------------------------------------------------------
void WrigglingSnake::CreateSnakeHead ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    // Create the snake head as a paraboloid that is attached to the last
    // ring of vertices on the snake body.  These vertices are generated
    // for t = 1.
    int numSliceSamples = mSnakeBody->GetNumSliceSamples();
    mSlice = new1<Vector3f>(numSliceSamples + 1);

    // Number of rays (determined by slice samples of tube surface).
    int numRays = numSliceSamples - 1;

    // Number of shells less one (your choice, specified in application
    // constructor).
    int numShellsM1 = mNumShells - 1;

    // Generate vertices (positions to be filled in by UpdateSnakeHead).
    int numVertices = 1 + numRays*numShellsM1;
    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);
    Float3 darkGreen(0.0f, 0.25f, 0.0f);
    for (int i = 0; i < numVertices; ++i)
    {
        vba.Color<Float3>(0, i) = darkGreen;
    }

    // Generate triangles.
    int numTriangles = numRays*(2*numShellsM1 - 1);
    int numIndices = 3*numTriangles;
    IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    for (int r0 = numRays - 1, r1 = 0, t = 0; r1 < numRays; r0 = r1++)
    {
        *indices++ = 0;
        *indices++ = 1 + numShellsM1*r0;
        *indices++ = 1 + numShellsM1*r1;
        ++t;
        for (int s = 1; s < numShellsM1; ++s)
        {
            int i00 = s + numShellsM1*r0;
            int i01 = s + numShellsM1*r1;
            int i10 = i00 + 1;
            int i11 = i01 + 1;
            *indices++ = i00;
            *indices++ = i10;
            *indices++ = i11;
            *indices++ = i00;
            *indices++ = i11;
            *indices++ = i01;
            t += 2;
        }
    }

    mSnakeHead = new0 TriMesh(vformat, vbuffer, ibuffer);
    mSnakeHead->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());
    mSnakeRoot->AttachChild(mSnakeHead);
    UpdateSnake();
}
//----------------------------------------------------------------------------
void WrigglingSnake::UpdateSnake ()
{
    // The snake head uses the last ring of vertices in the tube surface of
    // the snake body, so the body must be updated first.
    mSnakeBody->UpdateSurface();

    // Get the ring of vertices at the head-end of the tube.
    int numSliceSamples = mSnakeBody->GetNumSliceSamples();
    mSnakeBody->GetTMaxSlice(mSlice);

    // Compute the center of the slice vertices.
    Vector3f center = mSlice[0];
    int i;
    for (i = 1; i <= numSliceSamples; ++i)
    {
        center += mSlice[i];
    }
    center /= (float)(numSliceSamples + 1);

    // Compute a unit-length normal of the plane of the vertices.  The normal
    // points away from tube and is used to extrude the paraboloid surface
    // for the head.
    Vector3f edge1 = mSlice[1] - mSlice[0];
    Vector3f edge2 = mSlice[2] - mSlice[0];
    Vector3f normal = edge1.UnitCross(edge2);

    // Adjust the normal length to include the height of the ellipsoid vertex
    // above the plane of the slice.
    normal *= 3.0f*msRadius;

    // Set the head origin.
    VertexBufferAccessor vba(mSnakeHead);
    vba.Position<Vector3f>(0) = center + normal;

    // Set the remaining shells.
    const int numShellsM1 = mNumShells - 1;
    float factor = 1.0f/(float)numShellsM1;
    for (int r = 0; r < numSliceSamples-1; ++r)
    {
        for (int s = 1; s < mNumShells; ++s)
        {
            // The clamping shown here was necessary to avoid floating-point
            // errors in the release build of this application on Fedora 12
            // with
            //     gcc version 4.4.2 20091222 (Red Hat 4.4.2-20) (GCC)
            // For s = numShellsM1, t is just slightly larger than 1, even
            // though in theory t should be equal to 1.  The consequence was
            // that oneMinusT was just slightly negative (about -1e-08), which
            // caused Mathf::Pow to generate invalid outputs.  The debug build
            // of this application did not have this problem on Fedora 12.
            // None of the other platforms had this problem in debug or
            // release builds.
            float t = factor*s, oneMinusT;
            if (t <= 1.0f)
            {
                oneMinusT = 1.0f - t;
            }
            else
            {
                t = 1.0f;
                oneMinusT = 0.0f;
            }

            i = s + numShellsM1*r;
            vba.Position<Vector3f>(i) = oneMinusT*center +
                t*mSlice[r] + Mathf::Pow(oneMinusT, 0.25f)*normal;
        }
    }

    mSnakeHead->UpdateModelSpace(Visual::GU_MODEL_BOUND_ONLY);
    mRenderer->Update(mSnakeHead->GetVertexBuffer());

    mSnakeRoot->Update();
}
//----------------------------------------------------------------------------
void WrigglingSnake::ModifyCurve ()
{
    // Perturb the snake medial curve.
    float time = (float)GetTimeInSeconds();
    for (int i = 0; i < mNumCtrlPoints; ++i)
    {
        Vector3f ctrl = mCenter->GetControlPoint(i);
        ctrl.Z() = mAmplitudes[i]*Mathf::Sin(3.0f*time + mPhases[i]);
        mCenter->SetControlPoint(i, ctrl);
    }

    UpdateSnake();
}
//----------------------------------------------------------------------------
