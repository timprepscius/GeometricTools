// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2013/07/14)

#include "SkinnedBiped.h"

WM5_WINDOW_APPLICATION(SkinnedBiped);

//----------------------------------------------------------------------------
SkinnedBiped::SkinnedBiped ()
    :
    WindowApplication3("SampleGraphics/SkinnedBiped", 0, 0, 640, 480,
        Float4(0.5f, 0.0f, 1.0f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
    Environment::InsertDirectory(ThePath + "Data/");

    mAnimTime = 0.0;
    mAnimTimeDelta = 0.01;
}
//----------------------------------------------------------------------------
bool SkinnedBiped::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 1.0f, 1000.0f);
    APoint camPosition(80.0f, 0.0f, 23.0f);
    AVector camDVector(-1.0f, 0.0f, 0.0f);
    AVector camUVector(0.0f, 0.0f, 1.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    CreateScene();

    // Initial update of objects.
    mScene->Update();
    CopyNormalToTCoord1(mScene);

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

    InitializeCameraMotion(0.01f, 0.01f);
    InitializeObjectMotion(mScene);
    return true;
}
//----------------------------------------------------------------------------
void SkinnedBiped::OnTerminate ()
{
    mScene = 0;
    mWireState = 0;
    mLight = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void SkinnedBiped::OnIdle ()
{
    MeasureTime();

    if (MoveCamera())
    {
        mCuller.ComputeVisibleSet(mScene);
    }

    if (MoveObject())
    {
        mScene->Update();
        CopyNormalToTCoord1(mScene);
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
bool SkinnedBiped::OnKeyDown (unsigned char key, int x, int y)
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

    case 'g':
        mAnimTime += mAnimTimeDelta;
        mScene->Update(mAnimTime);
        CopyNormalToTCoord1(mScene);
        mCuller.ComputeVisibleSet(mScene);
        return true;
    case 'G':
        mAnimTime = 0.0;
        mScene->Update(mAnimTime);
        CopyNormalToTCoord1(mScene);
        mCuller.ComputeVisibleSet(mScene);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void SkinnedBiped::CreateScene ()
{
    // Allow for toggle of wireframe.
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    // The biped has materials assigned to its triangle meshes, so they need
    // lighting.
    mLight = new0 Light(Light::LT_DIRECTIONAL);
    mLight->Ambient = Float4(0.5f, 0.5f, 0.5f, 1.0f);
    mLight->Diffuse = mLight->Ambient;
    mLight->Specular = Float4(0.0f, 0.0f, 0.0f, 1.0f);
    mLight->Constant = 0.0f;
    mLight->Linear = 0.0f;
    mLight->Quadratic = 0.0f;
    mLight->Intensity = 1.0f;
    mLight->DVector = mCamera->GetDVector();

    Node* biped = GetNode("Biped");
    Node* pelvis = GetNode("Pelvis");
    Node* spine = GetNode("Spine");
    Node* spine1 = GetNode("Spine1");
    Node* spine2 = GetNode("Spine2");
    Node* spine3 = GetNode("Spine3");
    Node* neck = GetNode("Neck");
    Node* head = GetNode("Head");
    Node* leftClavicle = GetNode("L_Clavicle");
    Node* leftUpperArm = GetNode("L_UpperArm");
    Node* leftForeArm = GetNode("L_Forearm");
    Node* leftHand = GetNode("L_Hand");
    Node* rightClavicle = GetNode("R_Clavicle");
    Node* rightUpperArm = GetNode("R_UpperArm");
    Node* rightForeArm = GetNode("R_Forearm");
    Node* rightHand = GetNode("R_Hand");
    Node* leftThigh = GetNode("L_Thigh");
    Node* leftCalf = GetNode("L_Calf");
    Node* leftFoot = GetNode("L_Foot");
    Node* leftToe = GetNode("L_Toe");
    Node* rightThigh = GetNode("R_Thigh");
    Node* rightCalf = GetNode("R_Calf");
    Node* rightFoot = GetNode("R_Foot");
    Node* rightToe = GetNode("R_Toe");

    biped->AttachChild(pelvis);
        pelvis->AttachChild(spine);
            spine->AttachChild(spine1);
                spine1->AttachChild(spine2);
                    spine2->AttachChild(spine3);
                        spine3->AttachChild(neck);
                            neck->AttachChild(head);
                                // head->AttachChild(hair);
                            neck->AttachChild(leftClavicle);
                                leftClavicle->AttachChild(leftUpperArm);
                                    leftUpperArm->AttachChild(leftForeArm);
                                        leftForeArm->AttachChild(leftHand);
                                    // leftUpperArm->AttachChild(leftArm);
                            neck->AttachChild(rightClavicle);
                                rightClavicle->AttachChild(rightUpperArm);
                                    rightUpperArm->AttachChild(rightForeArm);
                                        rightForeArm->AttachChild(rightHand);
                                    // rightUpperArm->AttachChild(rightArm);
                        // spine3->AttachChild(face);
        pelvis->AttachChild(leftThigh);
            leftThigh->AttachChild(leftCalf);
                leftCalf->AttachChild(leftFoot);
                    leftFoot->AttachChild(leftToe);
                // leftCalf->AttachChild(leftShoe);
            // leftThigh->AttachChild(leftLeg);
            // leftThigh->AttachChild(leftAngle);
        pelvis->AttachChild(rightThigh);
            rightThigh->AttachChild(rightCalf);
                rightCalf->AttachChild(rightFoot);
                    rightFoot->AttachChild(rightToe);
                // rightCalf->AttachChild(rightShoe);
            // rightThigh->AttachChild(rightLeg);
            // rightThigh->AttachChild(rightAnkle);
        // pelvis->AttachChild(shirt);
        // pelvis->AttachChild(pants);


    // The vertex format is shared among all the triangle meshes.  The normals
    // are duplicated to texture coordinates to avoid the AMD lighting
    // problems due to use of pre-OpenGL2.x extensions.
    mVFormat = VertexFormat::Create(3,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_NORMAL, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT3, 1);

    // The TriMesh objects must be created after the Node tree is built,
    // because the TriMesh objects have to find the "bones" that correspond
    // to them.
    TriMesh* hair = GetMesh("Hair",biped);
    TriMesh* leftArm = GetMesh("L_Arm",biped);
    TriMesh* rightArm = GetMesh("R_Arm",biped);
    TriMesh* face = GetMesh("Face",biped);
    TriMesh* leftShoe = GetMesh("L_Shoe",biped);
    TriMesh* leftLeg = GetMesh("L_Leg",biped);
    TriMesh* leftAngle = GetMesh("L_Ankle",biped);
    TriMesh* rightShoe = GetMesh("R_Shoe",biped);
    TriMesh* rightLeg = GetMesh("R_Leg",biped);
    TriMesh* rightAnkle = GetMesh("R_Ankle",biped);
    TriMesh* shirt = GetMesh("Shirt",biped);
    TriMesh* pants = GetMesh("Pants",biped);

    head->AttachChild(hair);
    leftUpperArm->AttachChild(leftArm);
    rightUpperArm->AttachChild(rightArm);
    spine3->AttachChild(face);
    leftCalf->AttachChild(leftShoe);
    leftThigh->AttachChild(leftLeg);
    leftThigh->AttachChild(leftAngle);
    rightCalf->AttachChild(rightShoe);
    rightThigh->AttachChild(rightLeg);
    rightThigh->AttachChild(rightAnkle);
    pelvis->AttachChild(shirt);
    pelvis->AttachChild(pants);

    mScene = new0 Node();
    mScene->LocalTransform.SetRotate(HMatrix(AVector::UNIT_Z,
        0.25f*Mathf::PI));
    mScene->AttachChild(biped);
    mScene->Update();

#if 0
    // For regenerating the biped WMOF whenever engine streaming changes.
    OutStream target;
    target.Insert(mScene);
    target.Save("SkinnedBipedPNTC1.wmof");
#endif
}
//----------------------------------------------------------------------------
Node* SkinnedBiped::GetNode (const std::string& name)
{
    Node* node = new0 Node();
    node->SetName(name);

    // Load the keyframe controller.
    std::string filename = name + ".keyf.raw";
    std::string path = Environment::GetPathR(filename);
    FileIO inFile(path, FileIO::FM_DEFAULT_READ);

    int repeat;
    float minTime, maxTime, phase, frequency;
    inFile.Read(sizeof(float), &repeat);
    inFile.Read(sizeof(float), &minTime);
    inFile.Read(sizeof(float), &maxTime);
    inFile.Read(sizeof(float), &phase);
    inFile.Read(sizeof(float), &frequency);

    int numTranslations, numRotations, numScales;
    inFile.Read(sizeof(int), &numTranslations);
    inFile.Read(sizeof(int), &numRotations);
    inFile.Read(sizeof(int), &numScales);

    KeyframeController* ctrl = new0 KeyframeController(0, numTranslations,
        numRotations, numScales, Transform::IDENTITY);

    ctrl->Repeat = (Controller::RepeatType)repeat;
    ctrl->MinTime = (double)minTime;
    ctrl->MaxTime = (double)maxTime;
    ctrl->Phase = (double)phase;
    ctrl->Frequency = (double)frequency;

    if (numTranslations > 0)
    {
        float* translationTimes = ctrl->GetTranslationTimes();
        APoint* translations = ctrl->GetTranslations();
        inFile.Read(sizeof(float), numTranslations, translationTimes);
        for (int i = 0; i < numTranslations; ++i)
        {
            inFile.Read(sizeof(float), 3, &translations[i]);
            translations[i][3] = 1.0f;
        }
    }
    else
    {
        APoint translate;
        inFile.Read(sizeof(float), 3, &translate);
        node->LocalTransform.SetTranslate(translate);
    }

    if (numRotations > 0)
    {
        float* rotationTimes = ctrl->GetRotationTimes();
        HQuaternion* rotations = ctrl->GetRotations();
        inFile.Read(sizeof(float), numRotations, rotationTimes);
        inFile.Read(sizeof(float), 4*numRotations, rotations);
    }
    else
    {
        float entry[9];
        inFile.Read(sizeof(float), 9, entry);
        HMatrix rotate(
            entry[0], entry[1], entry[2], 0.0f,
            entry[3], entry[4], entry[5], 0.0f,
            entry[6], entry[7], entry[8], 0.0f,
            0.0f,     0.0f,     0.0f,     1.0f);
        node->LocalTransform.SetRotate(rotate);
    }

    if (numScales > 0)
    {
        float* scaleTimes = ctrl->GetScaleTimes();
        float* scales = ctrl->GetScales();
        inFile.Read(sizeof(float), numScales, scaleTimes);
        inFile.Read(sizeof(float), numScales, scales);
    }
    else
    {
        float scale;
        inFile.Read(sizeof(float), &scale);
        node->LocalTransform.SetUniformScale(scale);
    }

    ctrl->SetTransform(node->LocalTransform);

    inFile.Close();

    node->AttachController(ctrl);
    return node;
}
//----------------------------------------------------------------------------
TriMesh* SkinnedBiped::GetMesh (const std::string& name, Node* biped)
{
    // Load the triangle indices and material.
    std::string filename = name + ".triangle.raw";
    std::string path = Environment::GetPathR(filename);
    FileIO inFile(path, FileIO::FM_DEFAULT_READ);

    int numTriangles;
    inFile.Read(sizeof(int), &numTriangles);
    int numIndices = 3*numTriangles;
    IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    inFile.Read(sizeof(int), numIndices, indices);

    Material* material = new0 Material();
    Float4 emissive, ambient, diffuse, specular;
    inFile.Read(sizeof(float), 3, &material->Emissive);
    material->Emissive[3] = 1.0f;
    inFile.Read(sizeof(float), 3, &material->Ambient);
    material->Ambient[3] = 1.0f;
    inFile.Read(sizeof(float), 3, &material->Diffuse);
    material->Diffuse[3] = 1.0f;
    inFile.Read(sizeof(float), 3, &material->Specular);
    material->Specular[3] = 0.0f;

    inFile.Close();

    // Load the skin controller.
    filename = name + ".skin.raw";
    path = Environment::GetPathR(filename);
    inFile.Open(path, FileIO::FM_DEFAULT_READ);

    int repeat;
    float minTime, maxTime, phase, frequency;
    inFile.Read(sizeof(float), &repeat);
    inFile.Read(sizeof(float), &minTime);
    inFile.Read(sizeof(float), &maxTime);
    inFile.Read(sizeof(float), &phase);
    inFile.Read(sizeof(float), &frequency);

    int numVertices, numBones;
    inFile.Read(sizeof(int), &numVertices);
    inFile.Read(sizeof(int), &numBones);

    SkinController* ctrl = new0 SkinController(numVertices, numBones);

    ctrl->Repeat = (Controller::RepeatType)repeat;
    ctrl->MinTime = (double)minTime;
    ctrl->MaxTime = (double)maxTime;
    ctrl->Phase = (double)phase;
    ctrl->Frequency = (double)frequency;

    Node** bones = ctrl->GetBones();
    int i;
    for (i = 0; i < numBones; ++i)
    {
        int length;
        inFile.Read(sizeof(int), &length);
        char* boneName = new1<char>(length + 1);
        inFile.Read(sizeof(char), length, boneName);
        boneName[length] = 0;

        bones[i] = (Node*)biped->GetObjectByName(boneName);
        assertion(bones[i] != 0, "Failed to find bone.\n");
        delete1(boneName);
    }

    float** weights = ctrl->GetWeights();
    APoint** offsets = ctrl->GetOffsets();
    for (int j = 0; j < numVertices; ++j)
    {
        for (i = 0; i < numBones; ++i)
        {
            inFile.Read(sizeof(float), &weights[j][i]);
            inFile.Read(sizeof(float), 3, &offsets[j][i]);
        }
    }

    inFile.Close();

    int vstride = mVFormat->GetStride();
    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, vstride);

    // Set positions and normals to zero for now.  The controller update
    // will set the initial values.
    memset(vbuffer->GetData(), 0, numVertices*vstride);

    TriMesh* mesh = new0 TriMesh(mVFormat, vbuffer, ibuffer);
    mesh->SetName(name);
    mesh->AttachController(ctrl);
    mesh->SetEffectInstance(LightDirPerVerEffect::CreateUniqueInstance(
        mLight, material));

    return mesh;
}
//----------------------------------------------------------------------------
void SkinnedBiped::CopyNormalToTCoord1 (Object* object)
{
    TriMesh* mesh = DynamicCast<TriMesh>(object);
    if (mesh)
    {
        VertexBufferAccessor vba(mesh);
        for (int i = 0; i < vba.GetNumVertices(); ++i)
        {
            vba.TCoord<Vector3f>(1, i) = vba.Normal<Vector3f>(i);
        }
        mRenderer->Update(mesh->GetVertexBuffer());
    }

    Node* node = DynamicCast<Node>(object);
    if (node)
    {
        for (int i = 0; i < node->GetNumChildren(); ++i)
        {
            Spatial* child = node->GetChild(i);
            if (child)
            {
                CopyNormalToTCoord1(child);
            }
        }
    }
}
//----------------------------------------------------------------------------
