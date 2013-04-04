// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "MorphControllers.h"

WM5_WINDOW_APPLICATION(MorphControllers);

//----------------------------------------------------------------------------
MorphControllers::MorphControllers ()
    :
    WindowApplication3("SampleGraphics/MorphControllers", 0, 0, 640, 480,
        Float4(0.5f, 0.0f, 1.0f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
    Environment::InsertDirectory(ThePath + "Data/");

    mBaseTime = -1.0;
    mCurrTime = -1.0;
    mXPick = -1;
    mYPick = -1;
    sprintf(mPickMessage, "no selection");
    mPickPending = false;
    mCurrentFace = -1;
}
//----------------------------------------------------------------------------
bool MorphControllers::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 1000.0f);
    APoint camPosition(0.0f, 0.0f, 0.0f);
    AVector camDVector(1.0f, 0.0f, 0.0f);
    AVector camUVector(0.0f, 1.0f, 0.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    CreateScene();

    // Initial update of objects.
    mScene->Update();

    // The face is force culled, so there is no point in computing the visible
    // set at this time.
    mCuller.SetCamera(mCamera);

    InitializeCameraMotion(0.1f, 0.001f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void MorphControllers::OnTerminate ()
{
    mPicker.Records.clear();
    mScene = 0;
    mWireState = 0;
    mFace = 0;

    mMaterial = 0;
    mLight = 0;
    mIBuffer = 0;
    mEffect = 0;

    for (int i = 0; i < TARGET_QUANTITY; ++i)
    {
        mTarget[i] = 0;
    }

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void MorphControllers::OnIdle ()
{
    MeasureTime();

    MoveCamera();
    MoveObject();

    // Update the morph animation.
    if (mBaseTime == -1.0)
    {
        mBaseTime = GetTimeInSeconds();
        mCurrTime = mBaseTime;
    }
    else
    {
        mCurrTime = GetTimeInSeconds();
    }
    mScene->Update(mCurrTime - mBaseTime);
    mCuller.ComputeVisibleSet(mScene);

    if (mRenderer->PreDraw())
    {
        mRenderer->ClearBuffers();

        // Support for camera picking.
        APoint pos;
        AVector dir;
        bool pickingAttempted = false;

        // Draw the 5 targets in the upper half of the screen.
        int width = mRenderer->GetWidth();
        int height = mRenderer->GetHeight();
        int viewX, viewY, viewW, viewH;
        for (int i = 0; i < TARGET_QUANTITY; ++i)
        {
            viewX = (int)(0.2f*i*width);
            viewY = (int)(0.8f*height);
            viewW = (int)(0.2f*width);
            viewH = (int)(0.2f*height);
            mRenderer->SetViewport(viewX, viewY, viewW, viewH);

            mTarget[i]->Culling = Spatial::CULL_DYNAMIC;
            mCuller.ComputeVisibleSet(mScene);
            mRenderer->Draw(mCuller.GetVisibleSet());
            mTarget[i]->Culling = Spatial::CULL_ALWAYS;

            if (mPickPending)
            {
                if (mRenderer->GetPickRay(mXPick, mYPick, pos, dir))
                {
                    mPickPending = false;
                    pickingAttempted = true;
                    mPicker.Execute(mScene, pos, dir, 0.0f, Mathf::MAX_REAL);
                    if (mPicker.Records.size() > 0)
                    {
                        sprintf(mPickMessage, "target %d", i);
                    }
                }
            }
        }

        // Draw the animated face in the lower half of the screen.
        viewX = (int)(0.1f*width);
        viewY = 0;
        viewW = (int)(0.8f*width);
        viewH = (int)(0.8f*height);
        mRenderer->SetViewport(viewX, viewY, viewW, viewH);
        mFace->Culling = Spatial::CULL_DYNAMIC;
        mCuller.ComputeVisibleSet(mScene);
        mRenderer->Draw(mCuller.GetVisibleSet());
        mFace->Culling = Spatial::CULL_ALWAYS;

        if (mPickPending)
        {
            if (mRenderer->GetPickRay(mXPick, mYPick, pos, dir))
            {
                mPickPending = false;
                pickingAttempted = true;
                mPicker.Execute(mScene, pos, dir, 0.0f, Mathf::MAX_REAL);
                if (mPicker.Records.size() > 0)
                {
                    sprintf(mPickMessage, "morphed");
                }
            }
        }

        if (pickingAttempted && mPicker.Records.size() == 0)
        {
            sprintf(mPickMessage, "no selection");
        }

        mRenderer->SetViewport(0, 0, width, height);
        DrawFrameRate(8, GetHeight()-8, mTextColor);
        mRenderer->Draw(320, GetHeight()-8, mTextColor, mPickMessage);

        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool MorphControllers::OnKeyDown (unsigned char key, int x, int y)
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

    case ' ':
        ++mCurrentFace;
        if (mCurrentFace == 0)
        {
            mFace->Culling = Spatial::CULL_ALWAYS;
            mTarget[0]->Culling = Spatial::CULL_DYNAMIC;
        }
        else if (mCurrentFace == TARGET_QUANTITY)
        {
            mTarget[mCurrentFace-1]->Culling = Spatial::CULL_ALWAYS;
            mFace->Culling = Spatial::CULL_DYNAMIC;
            mCurrentFace = -1;
        }
        else
        {
            mTarget[mCurrentFace-1]->Culling = Spatial::CULL_ALWAYS;
            mTarget[mCurrentFace]->Culling = Spatial::CULL_DYNAMIC;
        }
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool MorphControllers::OnMouseClick (int button, int state, int x,
    int y, unsigned int)
{
    if (state != MOUSE_DOWN || button != MOUSE_LEFT_BUTTON)
    {
        return false;
    }

    mXPick = x;
    mYPick = GetHeight() - 1 - y;
    mPickPending = true;
    return true;
}
//----------------------------------------------------------------------------
void MorphControllers::CreateScene ()
{
    // Set up scene.  The transformations are selected using prior knowledge
    // of the input data sets.
    mScene = new0 Node();
    APoint translate(250.0f, 0.0f, 0.0f);
    HMatrix rotate(
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
    mScene->LocalTransform.SetRotate(rotate);
    mScene->LocalTransform.SetTranslate(translate);

    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    mMaterial = new0 Material();
    mMaterial->Emissive = Float4(0.0f, 0.0f, 0.0f, 1.0f);
    mMaterial->Ambient = Float4(0.5f, 0.5f, 0.5f, 1.0f);
    mMaterial->Diffuse = Float4(0.99607f, 0.83920f, 0.67059f, 1.0f);
    mMaterial->Specular = Float4(0.8f, 0.8f, 0.8f, 0.0f);

    mLight = new0 Light(Light::LT_DIRECTIONAL);
    mLight->Ambient = Float4(0.25f, 0.25f, 0.25f, 1.0f);
    mLight->Diffuse = Float4(1.0f, 1.0f, 1.0f, 1.0f);
    mLight->Specular = Float4(0.0f, 0.0f, 0.0f, 1.0f);
    mLight->Constant = 0.0f;
    mLight->Linear = 0.0f;
    mLight->Quadratic = 0.0f;
    mLight->Intensity = 1.0f;
    mLight->DVector = mCamera->GetDVector();

    LoadTargets();
    CreateFace();
    CreateController();

    // Attach the face to the scene.  Also attach the targets so we can
    // display them above the animated face.
    mFace->Culling = Spatial::CULL_ALWAYS;
    mScene->AttachChild(mFace);
    for (int i = 0; i < TARGET_QUANTITY; ++i)
    {
        mTarget[i]->Culling = Spatial::CULL_ALWAYS;
        mScene->AttachChild(mTarget[i]);
    }
}
//----------------------------------------------------------------------------
void MorphControllers::LoadTargets ()
{
    // Create triangle meshes for the targets just for the purposes of this
    // application.  The targets will be displayed directly above the animated
    // face.

    // Load triangle indices.
    std::string path = Environment::GetPathR("FaceIndices.raw");
    FileIO inFile(path, FileIO::FM_DEFAULT_READ);
    int numTriangles;
    inFile.Read(sizeof(int), &numTriangles);
    mIBuffer = new0 IndexBuffer(3*numTriangles, sizeof(int));
    int* indices = (int*)mIBuffer->GetData();
    int i;
    for (i = 0; i < numTriangles; ++i, indices += 3)
    {
        inFile.Read(sizeof(int), 3, indices);
    }
    inFile.Close();

    // Create a per-vertex light effect to be used by the targets.
    mEffect = new0 LightDirPerVerEffect();

    // Load the targets.  Create triangle meshes for each of the targets just
    // for the purposes of this application.  Normally target0 and the
    // differences between the other targets and target0 are stored only in
    // the controller.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_NORMAL, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    for (int j = 0; j < TARGET_QUANTITY; ++j)
    {
        char filename[64];
        sprintf(filename, "FaceTarget%d.raw", j);
        path = Environment::GetPathR(filename);
        inFile.Open(path, FileIO::FM_DEFAULT_READ);

        int numVertices;
        inFile.Read(sizeof(int), &numVertices);

        VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, vstride);
        VertexBufferAccessor vba(vformat, vbuffer);
        for (i = 0; i < numVertices; ++i)
        {
            Vector3f position;
            inFile.Read(sizeof(float), 3, &position);
            vba.Position<Vector3f>(i) = position;
        }

        inFile.Close();

        mTarget[j] = new0 TriMesh(vformat, vbuffer, mIBuffer);
        mTarget[j]->SetName(filename);
        mTarget[j]->UpdateModelSpace(Visual::GU_NORMALS);

        mTarget[j]->SetEffectInstance(mEffect->CreateInstance(mLight,
            mMaterial));
    }
}
//----------------------------------------------------------------------------
void MorphControllers::CreateFace ()
{
    // This is the triangle mesh that will be controlled.  Make a copy of the
    // vertices but share the vertex format and connectivity.

    VertexBuffer* trgVBuffer = mTarget[0]->GetVertexBuffer();
    VertexFormat* vformat = mTarget[0]->GetVertexFormat();
    IndexBuffer* ibuffer = mTarget[0]->GetIndexBuffer();

    VertexBuffer* vbuffer = new0 VertexBuffer(trgVBuffer->GetNumElements(),
        vformat->GetStride());
    memcpy(vbuffer->GetData(), trgVBuffer->GetData(),
        trgVBuffer->GetNumBytes());

    mFace = new0 TriMesh(vformat, vbuffer, ibuffer);
    mFace->UpdateModelSpace(Visual::GU_NORMALS);

#if 0
    // For regenerating the face WMOF whenever the engine streaming changes.
    OutStream target;
    target.Insert(mFace);
    target.Save("FacePN.wmof");
#endif

    mFace->SetName("original");

    mFace->SetEffectInstance(mEffect->CreateInstance(mLight, mMaterial));
}
//----------------------------------------------------------------------------
void MorphControllers::CreateController ()
{
    // Create the controller.  The parameters set here are known information
    // about the model.
    VertexBufferAccessor vba(mFace);
    int numVertices = vba.GetNumVertices();

    MorphController* ctrl = new0 MorphController(numVertices,
        TARGET_QUANTITY, KEY_QUANTITY);
    ctrl->Repeat = Controller::RT_WRAP;
    ctrl->MinTime = 0.0;
    ctrl->MaxTime = 16000.0;
    ctrl->Phase = 0.0;
    ctrl->Frequency = 4800.0;

    // Target 0 is the base mesh.
    APoint* base = ctrl->GetBaseVertices();
    int i;
    for (i = 0; i < numVertices; ++i)
    {
        base[i] = vba.Position<Float3>(i);
    }

    // The remaining targets are stored as differences from the base.
    for (int target = 1; target < TARGET_QUANTITY; ++target)
    {
        VertexBufferAccessor vgatarget(mTarget[target]);
        AVector* delta = ctrl->GetDeltaVertices(target);
        for (i = 0; i < numVertices; ++i)
        {
            APoint pos = vgatarget.Position<Float3>(i);
            delta[i] = pos - base[i];
        }
    }

    // Load the keys.
    float* times = ctrl->GetTimes();
    for (int key = 0; key < KEY_QUANTITY; ++key)
    {
        char filename[64];
        sprintf(filename, "FaceKey%d.raw", key);
        std::string path = Environment::GetPathR(filename);
        FileIO inFile(path, FileIO::FM_DEFAULT_READ);

        int numTargets;
        inFile.Read(sizeof(int), &numTargets);

        inFile.Read(sizeof(float), &times[key]);

        // Discard the first weight.  It is not needed because of the choice
        // to use base_target + difference_targets.
        float weight0;
        inFile.Read(sizeof(float), &weight0);

        float* weights = ctrl->GetWeights(key);
        inFile.Read(sizeof(float), numTargets-1, weights);

        inFile.Close();
    }

    mFace->AttachController(ctrl);
}
//----------------------------------------------------------------------------
