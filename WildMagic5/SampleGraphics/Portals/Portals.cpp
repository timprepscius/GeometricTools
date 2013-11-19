// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Portals.h"

WM5_WINDOW_APPLICATION(Portals);

//----------------------------------------------------------------------------
Portals::Portals ()
    :
    WindowApplication3("SampleGraphics/Portals", 0, 0, 640, 480,
        Float4(0.9f, 0.9f, 0.9f, 1.0f)),
        mTextColor(1.0f, 1.0f, 1.0f, 1.0f)
{
}
//----------------------------------------------------------------------------
bool Portals::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
    {
        return false;
    }

    // Set up the camera.
    mCamera->SetFrustum(60.0f, GetAspectRatio(), 0.1f, 100.0f);
    APoint camPosition(0.0f, 0.0f, 1.0f);
    AVector camDVector(0.25708461f, 0.96640458f, 0.0f);
    AVector camUVector(0.0f, 0.0f, 1.0f);
    AVector camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    CreateScene();

    // Initial update of objects.
    mScene->Update();

    // Initial culling of scene.
    mCuller.SetCamera(mCamera);
    mCuller.ComputeVisibleSet(mScene);

#if 1
    InitializeCameraMotion(0.001f, 0.001f);
#else
    // Enable this to see the problem with eyepoint outside the environment
    // but the near plane inside.  The initial configuration has the eyepoint
    // outside, but the near plane partially inside the environment.  Press
    // the UP-ARROW key a couple of times and see the inside pop into view.
    camPosition = APoint(-0.74392152f, -5.0354514f, 1.0f);
    camDVector = APoint(-0.65598810f, 0.75463903f, 0.0f);
    camUVector = APoint(0.0f, 0.0f, 1.0f);
    camRVector = camDVector.Cross(camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);
    InitializeCameraMotion(0.0001f, 0.001f);
#endif

    return true;
}
//----------------------------------------------------------------------------
void Portals::OnTerminate ()
{
    mScene = 0;
    mWireState = 0;

    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void Portals::OnIdle ()
{
    MeasureTime();

    if (MoveCamera())
    {
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
bool Portals::OnKeyDown (unsigned char key, int x, int y)
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
void Portals::CreateScene ()
{
    // Create the root of the scene.
    mScene = new0 Node();
    mWireState = new0 WireState();
    mRenderer->SetOverrideWireState(mWireState);

    // Load the textures.

    ConvexRegionManager* crm = CreateBspTree();
    crm->AttachOutside(CreateOutside());
    mScene->AttachChild(crm);
}
//----------------------------------------------------------------------------
ConvexRegionManager* Portals::CreateBspTree ()
{
    // Construction of the tree in BspTree.txt.  NOTE:  Not all HPlane normals
    // are unit length.  Even so, the BSP tree behaves properly.
    ConvexRegionManager* crm = new0 ConvexRegionManager(
        HPlane(0.0f, 0.0f, -1.0f, 0.0f));
    crm->SetName("CRM");

    BspNode* rNode = new0 BspNode(HPlane(0.0f, 0.0f, 1.0f, 2.0f));
    rNode->SetName("R");

    BspNode* rrNode = new0 BspNode(HPlane(1.0f, 0.0f, 0.0f, 1.0f));
    rrNode->SetName("RR");

    BspNode* rrlNode = new0 BspNode(HPlane(0.0f, 1.0f, 0.0f, 1.0f));
    rrlNode->SetName("RRL");

    BspNode* rrllNode = new0 BspNode(HPlane(1.0f, 1.0f, 0.0f, 4.0f));
    rrllNode->SetName("RRLL");

    BspNode* rrlllNode = new0 BspNode(HPlane(1.0f, 1.0f, 0.0f, 6.0f));
    rrlllNode->SetName("RRLLL");

    BspNode* rrlrNode = new0 BspNode(HPlane(0.0f, -1.0f, 0.0f, 1.0f));
    rrlrNode->SetName("RRLR");

    BspNode* rrlrlNode = new0 BspNode(HPlane(1.0f, -1.0f, 0.0f, 4.0f));
    rrlrlNode->SetName("RRLRL");

    BspNode* rrlrllNode = new0 BspNode(HPlane(1.0f, -1.0f, 0.0f, 6.0f));
    rrlrllNode->SetName("RRLRLL");

    BspNode* rrlrrNode = new0 BspNode(HPlane(1.0f, 0.0f, 0.0f, 3.0f));
    rrlrrNode->SetName("RRLRR");

    BspNode* rrlrrlNode = new0 BspNode(HPlane(1.0f, 0.0f, 0.0f, 5.0f));
    rrlrrlNode->SetName("RRLRRL");

    BspNode* rrrNode = new0 BspNode(HPlane(-1.0f, 0.0f, 0.0f, 1.0f));
    rrrNode->SetName("RRR");

    BspNode* rrrlNode = new0 BspNode(HPlane(0.0f, 1.0f, 0.0f, 1.0f));
    rrrlNode->SetName("RRRL");

    BspNode* rrrllNode = new0 BspNode(HPlane(-1.0f, 1.0f, 0.0f, 4.0f));
    rrrllNode->SetName("RRRLL");

    BspNode* rrrlllNode = new0 BspNode(HPlane(-1.0f, 1.0f, 0.0f, 6.0f));
    rrrlllNode->SetName("RRRLLL");

    BspNode* rrrlrNode = new0 BspNode(HPlane(0.0f, -1.0f, 0.0f, 1.0f));
    rrrlrNode->SetName("RRRLR");

    BspNode* rrrlrlNode = new0 BspNode(HPlane(-1.0f, -1.0f, 0.0f, 4.0f));
    rrrlrlNode->SetName("RRRLRL");

    BspNode* rrrlrllNode = new0 BspNode(HPlane(-1.0f, -1.0f, 0.0f, 6.0f));
    rrrlrllNode->SetName("RRRLRLL");

    BspNode* rrrlrrNode = new0 BspNode(HPlane(-1.0f, 0.0f, 0.0f, 3.0f));
    rrrlrrNode->SetName("RRRLRR");

    BspNode* rrrlrrlNode = new0 BspNode(HPlane(-1.0f, 0.0f, 0.0f, 5.0f));
    rrrlrrlNode->SetName("RRRLRRL");

    BspNode* rrrrNode = new0 BspNode(HPlane(0.0f, 1.0f, 0.0f, 1.0f));
    rrrrNode->SetName("RRRR");

    BspNode* rrrrlNode = new0 BspNode(HPlane(0.0f, 1.0f, 0.0f, 3.0f));
    rrrrlNode->SetName("RRRRL");

    BspNode* rrrrllNode = new0 BspNode(HPlane(0.0f, 1.0f, 0.0f, 5.0f));
    rrrrllNode->SetName("RRRRLL");

    BspNode* rrrrrNode = new0 BspNode(HPlane(0.0f, -1.0f, 0.0f, 1.0f));
    rrrrrNode->SetName("RRRRR");

    BspNode* rrrrrlNode = new0 BspNode(HPlane(0.0f, -1.0f, 0.0f, 3.0f));
    rrrrrlNode->SetName("RRRRRL");

    BspNode* rrrrrllNode = new0 BspNode(HPlane(0.0f, -1.0f, 0.0f, 5.0f));
    rrrrrllNode->SetName("RRRRRLL");

    // Load the textures.
    std::string path = Environment::GetPathR("Floor.wmtf");
    Texture2D* floor = Texture2D::LoadWMTF(path);
    path = Environment::GetPathR("Ceiling.wmtf");
    Texture2D* ceiling = Texture2D::LoadWMTF(path);
    path = Environment::GetPathR("Wall1.wmtf");
    Texture2D* wall1 = Texture2D::LoadWMTF(path);
    path = Environment::GetPathR("Wall2.wmtf");
    Texture2D* wall2 = Texture2D::LoadWMTF(path);
    path = Environment::GetPathR("Agate.wmtf");
    Texture2D* agate = Texture2D::LoadWMTF(path);
    path = Environment::GetPathR("Bark.wmtf");
    Texture2D* bark = Texture2D::LoadWMTF(path);
    path = Environment::GetPathR("RedSky.wmtf");
    Texture2D* sky = Texture2D::LoadWMTF(path);
    path = Environment::GetPathR("Water.wmtf");
    Texture2D* water = Texture2D::LoadWMTF(path);

    // The shared texture effect.
    mEffect = new0 Texture2DEffect(Shader::SF_LINEAR_LINEAR);

    // The shared vertex format for position-texture objects.
    mPTFormat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);
    mPTStride = mPTFormat->GetStride();

    // Create regions and portals.
    Node* node;
    Portal** portals;

    CreateCenterCube(floor, ceiling, wall1, water, agate, bark, sky, node,
        portals);
    ConvexRegion* c00 = new0 ConvexRegion(4, portals);
    c00->SetName("C00");
    c00->AttachChild(node);

    CreateAxisConnector(floor, ceiling, wall2, node, portals);
    ConvexRegion* c02 = new0 ConvexRegion(2, portals);
    c02->SetName("C02");
    c02->AttachChild(node);
    c02->LocalTransform.SetTranslate(APoint(0.0f, 2.0f, 0.0f));

    CreateAxisConnector(floor, ceiling, wall2, node, portals);
    ConvexRegion* c0m2 = new0 ConvexRegion(2, portals);
    c0m2->SetName("C0m2");
    c0m2->AttachChild(node);
    c0m2->LocalTransform.SetTranslate(APoint(0.0f, -2.0f, 0.0f));

    CreateAxisConnector(floor, ceiling, wall2, node, portals);
    ConvexRegion* c20 = new0 ConvexRegion(2, portals);
    c20->SetName("C20");
    c20->AttachChild(node);
    c20->LocalTransform.SetTranslate(APoint(2.0f, 0.0f, 0.0f));
    c20->LocalTransform.SetRotate(HMatrix(AVector::UNIT_Z, Mathf::HALF_PI));

    CreateAxisConnector(floor, ceiling, wall2, node, portals);
    ConvexRegion* cm20 = new0 ConvexRegion(2, portals);
    cm20->SetName("Cm20");
    cm20->AttachChild(node);
    cm20->LocalTransform.SetTranslate(APoint(-2.0f, 0.0f, 0.0f));
    cm20->LocalTransform.SetRotate(HMatrix(AVector::UNIT_Z, Mathf::HALF_PI));

    CreateEndCube(floor, ceiling, wall1, node, portals);
    ConvexRegion* c04 = new0 ConvexRegion(3, portals);
    c04->SetName("C04");
    c04->AttachChild(node);
    c04->LocalTransform.SetTranslate(APoint(0.0f, 4.0f, 0.0f));
    c04->LocalTransform.SetRotate(HMatrix(AVector::UNIT_Z, -Mathf::HALF_PI));

    CreateEndCube(floor, ceiling, wall1, node, portals);
    ConvexRegion* c0m4 = new0 ConvexRegion(3, portals);
    c0m4->SetName("C0m4");
    c0m4->AttachChild(node);
    c0m4->LocalTransform.SetTranslate(APoint(0.0f, -4.0f, 0.0f));
    c0m4->LocalTransform.SetRotate(HMatrix(AVector::UNIT_Z, Mathf::HALF_PI));

    CreateEndCube(floor, ceiling, wall1, node, portals);
    ConvexRegion* c40 = new0 ConvexRegion(3, portals);
    c40->SetName("C40");
    c40->AttachChild(node);
    c40->LocalTransform.SetTranslate(APoint(4.0f, 0.0f, 0.0f));
    c40->LocalTransform.SetRotate(HMatrix(AVector::UNIT_Z, Mathf::PI));

    CreateEndCube(floor, ceiling, wall1, node, portals);
    ConvexRegion* cm40 = new0 ConvexRegion(3, portals);
    cm40->SetName("Cm40");
    cm40->AttachChild(node);
    cm40->LocalTransform.SetTranslate(APoint(-4.0f, 0.0f, 0.0f));

    CreateDiagonalConnector(floor, ceiling, wall2, node, portals);
    ConvexRegion* dpp = new0 ConvexRegion(2, portals);
    dpp->SetName("DPP");
    dpp->AttachChild(node);

    CreateDiagonalConnector(floor, ceiling, wall2, node, portals);
    ConvexRegion* dmp = new0 ConvexRegion(2,portals);
    dmp->SetName("DMP");
    dmp->AttachChild(node);
    dmp->LocalTransform.SetRotate(HMatrix(AVector::UNIT_Z, Mathf::HALF_PI));

    CreateDiagonalConnector(floor, ceiling, wall2, node, portals);
    ConvexRegion* dmm = new0 ConvexRegion(2,portals);
    dmm->SetName("DMM");
    dmm->AttachChild(node);
    dmm->LocalTransform.SetRotate(HMatrix(AVector::UNIT_Z, Mathf::PI));

    CreateDiagonalConnector(floor, ceiling, wall2, node, portals);
    ConvexRegion* dpm = new0 ConvexRegion(2,portals);
    dpm->SetName("DPM");
    dpm->AttachChild(node);
    dpm->LocalTransform.SetRotate(HMatrix(AVector::UNIT_Z, -Mathf::HALF_PI));

    // Create the adjacency graph.
    c00->GetPortal(0)->AdjacentRegion = c0m2;
    c00->GetPortal(1)->AdjacentRegion = c02;
    c00->GetPortal(2)->AdjacentRegion = c20;
    c00->GetPortal(3)->AdjacentRegion = cm20;
    c02->GetPortal(0)->AdjacentRegion = c00;
    c02->GetPortal(1)->AdjacentRegion = c04;
    c0m2->GetPortal(0)->AdjacentRegion = c0m4;
    c0m2->GetPortal(1)->AdjacentRegion = c00;
    c20->GetPortal(0)->AdjacentRegion = c40;
    c20->GetPortal(1)->AdjacentRegion = c00;
    cm20->GetPortal(0)->AdjacentRegion = c00;
    cm20->GetPortal(1)->AdjacentRegion = cm40;
    c04->GetPortal(0)->AdjacentRegion = dmp;
    c04->GetPortal(1)->AdjacentRegion = dpp;
    c04->GetPortal(2)->AdjacentRegion = c02;
    c0m4->GetPortal(0)->AdjacentRegion = dpm;
    c0m4->GetPortal(1)->AdjacentRegion = dmm;
    c0m4->GetPortal(2)->AdjacentRegion = c0m2;
    c40->GetPortal(0)->AdjacentRegion = dpp;
    c40->GetPortal(1)->AdjacentRegion = dpm;
    c40->GetPortal(2)->AdjacentRegion = c20;
    cm40->GetPortal(0)->AdjacentRegion = dmm;
    cm40->GetPortal(1)->AdjacentRegion = dmp;
    cm40->GetPortal(2)->AdjacentRegion = cm20;
    dpp->GetPortal(0)->AdjacentRegion = c40;
    dpp->GetPortal(1)->AdjacentRegion = c04;
    dmp->GetPortal(0)->AdjacentRegion = c04;
    dmp->GetPortal(1)->AdjacentRegion = cm40;
    dmm->GetPortal(0)->AdjacentRegion = cm40;
    dmm->GetPortal(1)->AdjacentRegion = c0m4;
    dpm->GetPortal(0)->AdjacentRegion = c0m4;
    dpm->GetPortal(1)->AdjacentRegion = c40;

    // Create the BSP tree (depth-first attachment).
    crm->AttachPositiveChild(0);
    crm->AttachNegativeChild(rNode);
        rNode->AttachPositiveChild(0);
        rNode->AttachNegativeChild(rrNode);
            rrNode->AttachPositiveChild(rrlNode);
                rrlNode->AttachPositiveChild(rrllNode);
                    rrllNode->AttachPositiveChild(rrlllNode);
                        rrlllNode->AttachPositiveChild(0);
                        rrlllNode->AttachNegativeChild(dpp);
                    rrllNode->AttachNegativeChild(0);
                rrlNode->AttachNegativeChild(rrlrNode);
                    rrlrNode->AttachPositiveChild(rrlrlNode);
                        rrlrlNode->AttachPositiveChild(rrlrllNode);
                            rrlrllNode->AttachPositiveChild(0);
                            rrlrllNode->AttachNegativeChild(dpm);
                        rrlrlNode->AttachNegativeChild(0);
                    rrlrNode->AttachNegativeChild(rrlrrNode);
                        rrlrrNode->AttachPositiveChild(rrlrrlNode);
                            rrlrrlNode->AttachPositiveChild(0);
                            rrlrrlNode->AttachNegativeChild(c40);
                        rrlrrNode->AttachNegativeChild(c20);
            rrNode->AttachNegativeChild(rrrNode);
                rrrNode->AttachPositiveChild(rrrlNode);
                    rrrlNode->AttachPositiveChild(rrrllNode);
                        rrrllNode->AttachPositiveChild(rrrlllNode);
                            rrrlllNode->AttachPositiveChild(0);
                            rrrlllNode->AttachNegativeChild(dmp);
                        rrrllNode->AttachNegativeChild(0);
                    rrrlNode->AttachNegativeChild(rrrlrNode);
                        rrrlrNode->AttachPositiveChild(rrrlrlNode);
                            rrrlrlNode->AttachPositiveChild(rrrlrllNode);
                                rrrlrllNode->AttachPositiveChild(0);
                                rrrlrllNode->AttachNegativeChild(dmm);
                            rrrlrlNode->AttachNegativeChild(0);
                        rrrlrNode->AttachNegativeChild(rrrlrrNode);
                            rrrlrrNode->AttachPositiveChild(rrrlrrlNode);
                                rrrlrrlNode->AttachPositiveChild(0);
                                rrrlrrlNode->AttachNegativeChild(cm40);
                            rrrlrrNode->AttachNegativeChild(cm20);
                rrrNode->AttachNegativeChild(rrrrNode);
                    rrrrNode->AttachPositiveChild(rrrrlNode);
                        rrrrlNode->AttachPositiveChild(rrrrllNode);
                            rrrrllNode->AttachPositiveChild(0);
                            rrrrllNode->AttachNegativeChild(c04);
                        rrrrlNode->AttachNegativeChild(c02);
                    rrrrNode->AttachNegativeChild(rrrrrNode);
                        rrrrrNode->AttachPositiveChild(rrrrrlNode);
                            rrrrrlNode->AttachPositiveChild(rrrrrllNode);
                                rrrrrllNode->AttachPositiveChild(0);
                                rrrrrllNode->AttachNegativeChild(c0m4);
                            rrrrrlNode->AttachNegativeChild(c0m2);
                        rrrrrNode->AttachNegativeChild(c00);
    return crm;
}
//----------------------------------------------------------------------------
TriMesh* Portals::CreateOutside ()
{
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_COLOR, VertexFormat::AT_FLOAT3, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(40, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);
    vba.Position<Vector3f>(0) = Vector3f(-1.0f, -1.0f, 2.0f);
    vba.Position<Vector3f>(1) = Vector3f(+1.0f, -1.0f, 2.0f);
    vba.Position<Vector3f>(2) = Vector3f(+1.0f, +1.0f, 2.0f);
    vba.Position<Vector3f>(3) = Vector3f(-1.0f, +1.0f, 2.0f);
    vba.Position<Vector3f>(4) = Vector3f(-1.0f, -1.0f, 0.0f);
    vba.Position<Vector3f>(5) = Vector3f(+1.0f, -1.0f, 0.0f);
    vba.Position<Vector3f>(6) = Vector3f(+1.0f, +1.0f, 0.0f);
    vba.Position<Vector3f>(7) = Vector3f(-1.0f, +1.0f, 0.0f);

    Vector3f trn(4.0f, 0.0f, 0.0f);
    int i, j;
    for (i = 0, j = 8; i < 8; ++i, ++j)
    {
        vba.Position<Vector3f>(j) = vba.Position<Vector3f>(i) + trn;
    }

    trn = Vector3f(0.0f, 4.0f, 0.0f);
    for (i = 0, j = 16; i < 8; ++i, ++j)
    {
        vba.Position<Vector3f>(j) = vba.Position<Vector3f>(i) + trn;
    }

    trn = Vector3f(-4.0f, 0.0f, 0.0f);
    for (i = 0, j = 24; i < 8; ++i, ++j)
    {
        vba.Position<Vector3f>(j) = vba.Position<Vector3f>(i) + trn;
    }

    trn = Vector3f(0.0f, -4.0f, 0.0f);
    for (i = 0, j = 32; i < 8; ++i, ++j)
    {
        vba.Position<Vector3f>(j) = vba.Position<Vector3f>(i) + trn;
    }

    for (i = 0; i < 40; ++i)
    {
        vba.Color<Float3>(0, i) = Float3(Mathf::UnitRandom(),
            Mathf::UnitRandom(), Mathf::UnitRandom());
    }

    IndexBuffer* ibuffer = new0 IndexBuffer(276, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[ 0] =  0;  indices[ 1] =  1;  indices[ 2] =  2;
    indices[ 3] =  0;  indices[ 4] =  2;  indices[ 5] =  3;
    indices[ 6] =  1;  indices[ 7] = 11;  indices[ 8] =  2;
    indices[ 9] =  1;  indices[10] =  8;  indices[11] = 11;
    indices[12] =  8;  indices[13] = 10;  indices[14] = 11;
    indices[15] =  8;  indices[16] =  9;  indices[17] = 10;
    indices[18] = 25;  indices[19] =  0;  indices[20] =  3;
    indices[21] = 25;  indices[22] =  3;  indices[23] = 26;
    indices[24] = 24;  indices[25] = 25;  indices[26] = 26;
    indices[27] = 24;  indices[28] = 26;  indices[29] = 27;
    indices[30] =  3;  indices[31] =  2;  indices[32] = 17;
    indices[33] =  3;  indices[34] = 17;  indices[35] = 16;
    indices[36] = 16;  indices[37] = 17;  indices[38] = 18;
    indices[39] = 16;  indices[40] = 18;  indices[41] = 19;
    indices[42] = 35;  indices[43] =  1;  indices[44] =  0;
    indices[45] = 35;  indices[46] = 34;  indices[47] =  1;
    indices[48] = 32;  indices[49] = 34;  indices[50] = 35;
    indices[51] = 32;  indices[52] = 33;  indices[53] = 34;
    indices[54] = 27;  indices[55] = 16;  indices[56] = 19;
    indices[57] = 27;  indices[58] = 26;  indices[59] = 16;
    indices[60] = 17;  indices[61] = 11;  indices[62] = 18;
    indices[63] = 18;  indices[64] = 11;  indices[65] = 10;
    indices[66] = 24;  indices[67] = 32;  indices[68] = 25;
    indices[69] = 25;  indices[70] = 32;  indices[71] = 35;
    indices[72] = 34;  indices[73] =  9;  indices[74] =  8;
    indices[75] = 34;  indices[76] = 33;  indices[77] =  9;

    for (i = 0, j = 78; i < 26; ++i)
    {
        indices[j++] = 4 + indices[3*i    ];
        indices[j++] = 4 + indices[3*i + 2];
        indices[j++] = 4 + indices[3*i + 1];
    }

    indices[156] =  6;  indices[157] =  2;  indices[158] = 11;
    indices[159] =  6;  indices[160] = 11;  indices[161] = 15;
    indices[162] = 21;  indices[163] = 17;  indices[164] =  2;
    indices[165] = 21;  indices[166] =  2;  indices[167] =  6;
    indices[168] = 15;  indices[169] = 11;  indices[170] = 17;
    indices[171] = 15;  indices[172] = 17;  indices[173] = 21;
    indices[174] = 12;  indices[175] =  8;  indices[176] =  1;
    indices[177] = 12;  indices[178] =  1;  indices[179] =  5;
    indices[180] =  5;  indices[181] =  1;  indices[182] = 34;
    indices[183] =  5;  indices[184] = 34;  indices[185] = 38;
    indices[186] = 38;  indices[187] = 34;  indices[188] =  8;
    indices[189] = 38;  indices[190] =  8;  indices[191] = 12;
    indices[192] =  4;  indices[193] =  0;  indices[194] = 25;
    indices[195] =  4;  indices[196] = 25;  indices[197] = 29;
    indices[198] = 39;  indices[199] = 35;  indices[200] =  0;
    indices[201] = 39;  indices[202] =  0;  indices[203] =  4;
    indices[204] = 29;  indices[205] = 25;  indices[206] = 35;
    indices[207] = 29;  indices[208] = 35;  indices[209] = 39;
    indices[210] =  7;  indices[211] =  3;  indices[212] = 16;
    indices[213] =  7;  indices[214] = 16;  indices[215] = 20;
    indices[216] = 30;  indices[217] = 26;  indices[218] =  3;
    indices[219] = 30;  indices[220] =  3;  indices[221] =  7;
    indices[222] = 20;  indices[223] = 16;  indices[224] = 26;
    indices[225] = 20;  indices[226] = 26;  indices[227] = 30;
    indices[228] = 14;  indices[229] = 10;  indices[230] =  9;
    indices[231] = 14;  indices[232] =  9;  indices[233] = 13;
    indices[234] = 22;  indices[235] = 18;  indices[236] = 10;
    indices[237] = 22;  indices[238] = 10;  indices[239] = 14;
    indices[240] = 23;  indices[241] = 19;  indices[242] = 18;
    indices[243] = 23;  indices[244] = 18;  indices[245] = 22;
    indices[246] = 31;  indices[247] = 27;  indices[248] = 19;
    indices[249] = 31;  indices[250] = 19;  indices[251] = 23;
    indices[252] = 28;  indices[253] = 24;  indices[254] = 27;
    indices[255] = 28;  indices[256] = 27;  indices[257] = 31;
    indices[258] = 36;  indices[259] = 32;  indices[260] = 24;
    indices[261] = 36;  indices[262] = 24;  indices[263] = 28;
    indices[264] = 37;  indices[265] = 33;  indices[266] = 32;
    indices[267] = 37;  indices[268] = 32;  indices[269] = 36;
    indices[270] = 13;  indices[271] =  9;  indices[272] = 33;
    indices[273] = 13;  indices[274] = 33;  indices[275] = 37;

    TriMesh* mesh = new0 TriMesh(vformat, vbuffer, ibuffer);
    mesh->SetEffectInstance(VertexColor3Effect::CreateUniqueInstance());
    return mesh;
}
//----------------------------------------------------------------------------
void Portals::CreateCenterCube (Texture2D* floorTexture,
    Texture2D* ceilingTexture, Texture2D* wallTexture,
    Texture2D* picture0Texture, Texture2D* picture1Texture,
    Texture2D* picture2Texture, Texture2D* picture3Texture, Node*& cube,
    Portal**& portals)
{
    cube = new0 Node();

    // walls
    VertexBuffer* vbuffer = new0 VertexBuffer(24, mPTStride);
    VertexBufferAccessor vba(mPTFormat, vbuffer);
    vba.Position<Vector3f>( 0) = Vector3f(-1.0f, -1.0f, 0.0f);
    vba.Position<Vector3f>( 1) = Vector3f(+1.0f, -1.0f, 0.0f);
    vba.Position<Vector3f>( 2) = Vector3f(+1.0f, +1.0f, 0.0f);
    vba.Position<Vector3f>( 3) = Vector3f(-1.0f, +1.0f, 0.0f);
    vba.Position<Vector3f>( 4) = Vector3f(-1.0f, -1.0f, 2.0f);
    vba.Position<Vector3f>( 5) = Vector3f(+1.0f, -1.0f, 2.0f);
    vba.Position<Vector3f>( 6) = Vector3f(+1.0f, +1.0f, 2.0f);
    vba.Position<Vector3f>( 7) = Vector3f(-1.0f, +1.0f, 2.0f);
    vba.Position<Vector3f>( 8) = Vector3f(+0.5f, -1.0f, 0.0f);
    vba.Position<Vector3f>( 9) = Vector3f(+0.5f, -1.0f, 1.5f);
    vba.Position<Vector3f>(10) = Vector3f(-0.5f, -1.0f, 1.5f);
    vba.Position<Vector3f>(11) = Vector3f(-0.5f, -1.0f, 0.0f);
    vba.Position<Vector3f>(12) = Vector3f(-0.5f, +1.0f, 0.0f);
    vba.Position<Vector3f>(13) = Vector3f(-0.5f, +1.0f, 1.5f);
    vba.Position<Vector3f>(14) = Vector3f(+0.5f, +1.0f, 1.5f);
    vba.Position<Vector3f>(15) = Vector3f(+0.5f, +1.0f, 0.0f);
    vba.Position<Vector3f>(16) = Vector3f(+1.0f, +0.5f, 0.0f);
    vba.Position<Vector3f>(17) = Vector3f(+1.0f, +0.5f, 1.5f);
    vba.Position<Vector3f>(18) = Vector3f(+1.0f, -0.5f, 1.5f);
    vba.Position<Vector3f>(19) = Vector3f(+1.0f, -0.5f, 0.0f);
    vba.Position<Vector3f>(20) = Vector3f(-1.0f, -0.5f, 0.0f);
    vba.Position<Vector3f>(21) = Vector3f(-1.0f, -0.5f, 1.5f);
    vba.Position<Vector3f>(22) = Vector3f(-1.0f, +0.5f, 1.5f);
    vba.Position<Vector3f>(23) = Vector3f(-1.0f, +0.5f, 0.0f);
    vba.TCoord<Float2>(0,  0) = Float2(0.00f, 0.00f);
    vba.TCoord<Float2>(0,  1) = Float2(1.00f, 0.00f);
    vba.TCoord<Float2>(0,  2) = Float2(0.00f, 0.00f);
    vba.TCoord<Float2>(0,  3) = Float2(1.00f, 0.00f);
    vba.TCoord<Float2>(0,  4) = Float2(0.00f, 1.00f);
    vba.TCoord<Float2>(0,  5) = Float2(1.00f, 1.00f);
    vba.TCoord<Float2>(0,  6) = Float2(0.00f, 1.00f);
    vba.TCoord<Float2>(0,  7) = Float2(1.00f, 1.00f);
    vba.TCoord<Float2>(0,  8) = Float2(0.75f, 0.00f);
    vba.TCoord<Float2>(0,  9) = Float2(0.75f, 0.75f);
    vba.TCoord<Float2>(0, 10) = Float2(0.25f, 0.75f);
    vba.TCoord<Float2>(0, 11) = Float2(0.25f, 0.00f);
    vba.TCoord<Float2>(0, 12) = Float2(0.75f, 0.00f);
    vba.TCoord<Float2>(0, 13) = Float2(0.75f, 0.75f);
    vba.TCoord<Float2>(0, 14) = Float2(0.25f, 0.75f);
    vba.TCoord<Float2>(0, 15) = Float2(0.25f, 0.00f);
    vba.TCoord<Float2>(0, 16) = Float2(0.25f, 0.00f);
    vba.TCoord<Float2>(0, 17) = Float2(0.25f, 0.75f);
    vba.TCoord<Float2>(0, 18) = Float2(0.75f, 0.75f);
    vba.TCoord<Float2>(0, 19) = Float2(0.75f, 0.00f);
    vba.TCoord<Float2>(0, 20) = Float2(0.25f, 0.00f);
    vba.TCoord<Float2>(0, 21) = Float2(0.25f, 0.75f);
    vba.TCoord<Float2>(0, 22) = Float2(0.75f, 0.75f);
    vba.TCoord<Float2>(0, 23) = Float2(0.75f, 0.00f);

    IndexBuffer* ibuffer = new0 IndexBuffer(72, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[ 0] =  1;  indices[ 1] =  8;  indices[ 2] =  9;
    indices[ 3] =  1;  indices[ 4] =  9;  indices[ 5] =  5;
    indices[ 6] =  5;  indices[ 7] =  9;  indices[ 8] =  4;
    indices[ 9] =  9;  indices[10] = 10;  indices[11] =  4;
    indices[12] = 10;  indices[13] =  0;  indices[14] =  4;
    indices[15] = 11;  indices[16] =  0;  indices[17] = 10;
    indices[18] =  3;  indices[19] = 12;  indices[20] = 13;
    indices[21] =  3;  indices[22] = 13;  indices[23] =  7;
    indices[24] =  7;  indices[25] = 13;  indices[26] =  6;
    indices[27] = 13;  indices[28] = 14;  indices[29] =  6;
    indices[30] = 14;  indices[31] =  2;  indices[32] =  6;
    indices[33] = 15;  indices[34] =  2;  indices[35] = 14;
    indices[36] =  2;  indices[37] = 16;  indices[38] = 17;
    indices[39] =  2;  indices[40] = 17;  indices[41] =  6;
    indices[42] =  6;  indices[43] = 17;  indices[44] =  5;
    indices[45] = 17;  indices[46] = 18;  indices[47] =  5;
    indices[48] = 18;  indices[49] =  1;  indices[50] =  5;
    indices[51] = 19;  indices[52] =  1;  indices[53] = 18;
    indices[54] =  0;  indices[55] = 20;  indices[56] = 21;
    indices[57] =  0;  indices[58] = 21;  indices[59] =  4;
    indices[60] =  4;  indices[61] = 21;  indices[62] =  7;
    indices[63] = 21;  indices[64] = 22;  indices[65] =  7;
    indices[66] = 22;  indices[67] =  3;  indices[68] =  7;
    indices[69] = 23;  indices[70] =  3;  indices[71] = 22;

    TriMesh* mesh = new0 TriMesh(mPTFormat, vbuffer, ibuffer);
    mesh->SetEffectInstance(mEffect->CreateInstance(wallTexture));
    cube->AttachChild(mesh);

    // portals
    portals = new1<Portal*>(4);

    APoint* modelVertices = new1<APoint>(4);
    modelVertices[0] = vba.Position<Vector3f>(11);
    modelVertices[1] = vba.Position<Vector3f>(10);
    modelVertices[2] = vba.Position<Vector3f>( 9);
    modelVertices[3] = vba.Position<Vector3f>( 8);
    HPlane modelPlane(modelVertices[0], modelVertices[1], modelVertices[2]);
    portals[0] = new0 Portal(4, modelVertices, modelPlane, 0, true);

    modelVertices = new1<APoint>(4);
    modelVertices[0] = vba.Position<Vector3f>(15);
    modelVertices[1] = vba.Position<Vector3f>(14);
    modelVertices[2] = vba.Position<Vector3f>(13);
    modelVertices[3] = vba.Position<Vector3f>(12);
    modelPlane = HPlane(modelVertices[0], modelVertices[1], modelVertices[2]);
    portals[1] = new0 Portal(4, modelVertices, modelPlane, 0, true);

    modelVertices = new1<APoint>(4);
    modelVertices[0] = vba.Position<Vector3f>(19);
    modelVertices[1] = vba.Position<Vector3f>(18);
    modelVertices[2] = vba.Position<Vector3f>(17);
    modelVertices[3] = vba.Position<Vector3f>(16);
    modelPlane = HPlane(modelVertices[0], modelVertices[1], modelVertices[2]);
    portals[2] = new0 Portal(4, modelVertices, modelPlane, 0, true);

    modelVertices = new1<APoint>(4);
    modelVertices[0] = vba.Position<Vector3f>(23);
    modelVertices[1] = vba.Position<Vector3f>(22);
    modelVertices[2] = vba.Position<Vector3f>(21);
    modelVertices[3] = vba.Position<Vector3f>(20);
    modelPlane = HPlane(modelVertices[0], modelVertices[1], modelVertices[2]);
    portals[3] = new0 Portal(4, modelVertices, modelPlane, 0, true);

    // floor
    vbuffer = new0 VertexBuffer(4, mPTStride);
    vba.ApplyTo(mPTFormat, vbuffer);
    vba.Position<Vector3f>(0) = Vector3f(-1.0f, -1.0f, 0.0f);
    vba.Position<Vector3f>(1) = Vector3f(+1.0f, -1.0f, 0.0f);
    vba.Position<Vector3f>(2) = Vector3f(+1.0f, +1.0f, 0.0f);
    vba.Position<Vector3f>(3) = Vector3f(-1.0f, +1.0f, 0.0f);
    vba.TCoord<Float2>(0, 0) = Float2(0.0f, 0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(1.0f, 0.0f);
    vba.TCoord<Float2>(0, 2) = Float2(1.0f, 1.0f);
    vba.TCoord<Float2>(0, 3) = Float2(0.0f, 1.0f);

    ibuffer = new0 IndexBuffer(6, sizeof(int));
    indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
    indices[3] = 0;  indices[4] = 2;  indices[5] = 3;

    mesh = new0 TriMesh(mPTFormat, vbuffer, ibuffer);
    mesh->SetEffectInstance(mEffect->CreateInstance(floorTexture));
    cube->AttachChild(mesh);

    // ceiling
    vbuffer = new0 VertexBuffer(4, mPTStride);
    vba.ApplyTo(mPTFormat, vbuffer);
    vba.Position<Vector3f>(0) = Vector3f(-1.0f, -1.0f, 2.0f);
    vba.Position<Vector3f>(1) = Vector3f(+1.0f, -1.0f, 2.0f);
    vba.Position<Vector3f>(2) = Vector3f(+1.0f, +1.0f, 2.0f);
    vba.Position<Vector3f>(3) = Vector3f(-1.0f, +1.0f, 2.0f);
    vba.TCoord<Float2>(0, 0) = Float2(0.0f, 0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(1.0f, 0.0f);
    vba.TCoord<Float2>(0, 2) = Float2(1.0f, 1.0f);
    vba.TCoord<Float2>(0, 3) = Float2(0.0f, 1.0f);

    ibuffer = new0 IndexBuffer(6, sizeof(int));
    indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 2;  indices[2] = 1;
    indices[3] = 0;  indices[4] = 3;  indices[5] = 2;

    mesh = new0 TriMesh(mPTFormat, vbuffer, ibuffer);
    mesh->SetEffectInstance(mEffect->CreateInstance(ceilingTexture));
    cube->AttachChild(mesh);

    // object 0
    vbuffer = new0 VertexBuffer(4, mPTStride);
    vba.ApplyTo(mPTFormat, vbuffer);
    vba.Position<Vector3f>(0) = Vector3f(-0.875f, -0.625f, 0.75f);
    vba.Position<Vector3f>(1) = Vector3f(-0.875f, -0.625f, 1.10f);
    vba.Position<Vector3f>(2) = Vector3f(-0.625f, -0.875f, 1.10f);
    vba.Position<Vector3f>(3) = Vector3f(-0.625f, -0.875f, 0.75f);
    vba.TCoord<Float2>(0, 0) = Float2(1.0f, 0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(1.0f, 1.0f);
    vba.TCoord<Float2>(0, 2) = Float2(0.0f, 1.0f);
    vba.TCoord<Float2>(0, 3) = Float2(0.0f, 0.0f);

    ibuffer = new0 IndexBuffer(6, sizeof(int));
    indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
    indices[3] = 0;  indices[4] = 2;  indices[5] = 3;

    mesh = new0 TriMesh(mPTFormat, vbuffer, ibuffer);
    mesh->SetEffectInstance(mEffect->CreateInstance(picture0Texture));
    cube->AttachChild(mesh);

    // object 1
    vbuffer = new0 VertexBuffer(4, mPTStride);
    vba.ApplyTo(mPTFormat, vbuffer);
    vba.Position<Vector3f>(0) = Vector3f(+0.875f, +0.625f, 0.75f);
    vba.Position<Vector3f>(1) = Vector3f(+0.875f, +0.625f, 1.10f);
    vba.Position<Vector3f>(2) = Vector3f(+0.625f, +0.875f, 1.10f);
    vba.Position<Vector3f>(3) = Vector3f(+0.625f, +0.875f, 0.75f);
    vba.TCoord<Float2>(0, 0) = Float2(1.0f, 0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(1.0f, 1.0f);
    vba.TCoord<Float2>(0, 2) = Float2(0.0f, 1.0f);
    vba.TCoord<Float2>(0, 3) = Float2(0.0f, 0.0f);

    ibuffer = new0 IndexBuffer(6, sizeof(int));
    indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
    indices[3] = 0;  indices[4] = 2;  indices[5] = 3;

    mesh = new0 TriMesh(mPTFormat, vbuffer, ibuffer);
    mesh->SetEffectInstance(mEffect->CreateInstance(picture1Texture));
    cube->AttachChild(mesh);

    // object 2
    vbuffer = new0 VertexBuffer(4, mPTStride);
    vba.ApplyTo(mPTFormat, vbuffer);
    vba.Position<Vector3f>(0) = Vector3f(-0.625f, +0.875f, 0.75f);
    vba.Position<Vector3f>(1) = Vector3f(-0.625f, +0.875f, 1.10f);
    vba.Position<Vector3f>(2) = Vector3f(-0.875f, +0.625f, 1.10f);
    vba.Position<Vector3f>(3) = Vector3f(-0.875f, +0.625f, 0.75f);
    vba.TCoord<Float2>(0, 0) = Float2(1.0f, 0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(1.0f, 1.0f);
    vba.TCoord<Float2>(0, 2) = Float2(0.0f, 1.0f);
    vba.TCoord<Float2>(0, 3) = Float2(0.0f, 0.0f);

    ibuffer = new0 IndexBuffer(6, sizeof(int));
    indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
    indices[3] = 0;  indices[4] = 2;  indices[5] = 3;

    mesh = new0 TriMesh(mPTFormat, vbuffer, ibuffer);
    mesh->SetEffectInstance(mEffect->CreateInstance(picture2Texture));
    cube->AttachChild(mesh);

    // object 3
    vbuffer = new0 VertexBuffer(4, mPTStride);
    vba.ApplyTo(mPTFormat, vbuffer);
    vba.Position<Vector3f>(0) = Vector3f(+0.625f, -0.875f, 0.75f);
    vba.Position<Vector3f>(1) = Vector3f(+0.625f, -0.875f, 1.10f);
    vba.Position<Vector3f>(2) = Vector3f(+0.875f, -0.625f, 1.10f);
    vba.Position<Vector3f>(3) = Vector3f(+0.875f, -0.625f, 0.75f);
    vba.TCoord<Float2>(0, 0) = Float2(1.0f, 0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(1.0f, 1.0f);
    vba.TCoord<Float2>(0, 2) = Float2(0.0f, 1.0f);
    vba.TCoord<Float2>(0, 3) = Float2(0.0f, 0.0f);

    ibuffer = new0 IndexBuffer(6, sizeof(int));
    indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
    indices[3] = 0;  indices[4] = 2;  indices[5] = 3;

    mesh = new0 TriMesh(mPTFormat, vbuffer, ibuffer);
    mesh->SetEffectInstance(mEffect->CreateInstance(picture3Texture));
    cube->AttachChild(mesh);
}
//----------------------------------------------------------------------------
void Portals::CreateAxisConnector (Texture2D* floorTexture,
    Texture2D* ceilingTexture, Texture2D* wallTexture, Node*& cube,
    Portal**& portals)
{
    cube = new0 Node();

    // walls
    VertexBuffer* vbuffer = new0 VertexBuffer(16, mPTStride);
    VertexBufferAccessor vba(mPTFormat, vbuffer);
    vba.Position<Vector3f>( 0) = Vector3f(-1.0f, -1.0f, 0.0f);
    vba.Position<Vector3f>( 1) = Vector3f(+1.0f, -1.0f, 0.0f);
    vba.Position<Vector3f>( 2) = Vector3f(+1.0f, +1.0f, 0.0f);
    vba.Position<Vector3f>( 3) = Vector3f(-1.0f, +1.0f, 0.0f);
    vba.Position<Vector3f>( 4) = Vector3f(-1.0f, -1.0f, 2.0f);
    vba.Position<Vector3f>( 5) = Vector3f(+1.0f, -1.0f, 2.0f);
    vba.Position<Vector3f>( 6) = Vector3f(+1.0f, +1.0f, 2.0f);
    vba.Position<Vector3f>( 7) = Vector3f(-1.0f, +1.0f, 2.0f);
    vba.Position<Vector3f>( 8) = Vector3f(+0.5f, -1.0f, 0.0f);
    vba.Position<Vector3f>( 9) = Vector3f(+0.5f, -1.0f, 1.5f);
    vba.Position<Vector3f>(10) = Vector3f(-0.5f, -1.0f, 1.5f);
    vba.Position<Vector3f>(11) = Vector3f(-0.5f, -1.0f, 0.0f);
    vba.Position<Vector3f>(12) = Vector3f(-0.5f, +1.0f, 0.0f);
    vba.Position<Vector3f>(13) = Vector3f(-0.5f, +1.0f, 1.5f);
    vba.Position<Vector3f>(14) = Vector3f(+0.5f, +1.0f, 1.5f);
    vba.Position<Vector3f>(15) = Vector3f(+0.5f, +1.0f, 0.0f);
    vba.TCoord<Float2>(0,  0) = Float2(0.00f, 0.00f);
    vba.TCoord<Float2>(0,  1) = Float2(1.00f, 0.00f);
    vba.TCoord<Float2>(0,  2) = Float2(0.00f, 0.00f);
    vba.TCoord<Float2>(0,  3) = Float2(1.00f, 0.00f);
    vba.TCoord<Float2>(0,  4) = Float2(0.00f, 1.00f);
    vba.TCoord<Float2>(0,  5) = Float2(1.00f, 1.00f);
    vba.TCoord<Float2>(0,  6) = Float2(0.00f, 1.00f);
    vba.TCoord<Float2>(0,  7) = Float2(1.00f, 1.00f);
    vba.TCoord<Float2>(0,  8) = Float2(0.75f, 0.00f);
    vba.TCoord<Float2>(0,  9) = Float2(0.75f, 0.75f);
    vba.TCoord<Float2>(0, 10) = Float2(0.25f, 0.75f);
    vba.TCoord<Float2>(0, 11) = Float2(0.25f, 0.00f);
    vba.TCoord<Float2>(0, 12) = Float2(0.75f, 0.00f);
    vba.TCoord<Float2>(0, 13) = Float2(0.75f, 0.75f);
    vba.TCoord<Float2>(0, 14) = Float2(0.25f, 0.75f);
    vba.TCoord<Float2>(0, 15) = Float2(0.25f, 0.00f);

    IndexBuffer* ibuffer = new0 IndexBuffer(48, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[ 0] =  1;  indices[ 1] =  8;  indices[ 2] =  9;
    indices[ 3] =  1;  indices[ 4] =  9;  indices[ 5] =  5;
    indices[ 6] =  5;  indices[ 7] =  9;  indices[ 8] =  4;
    indices[ 9] =  9;  indices[10] = 10;  indices[11] =  4;
    indices[12] = 10;  indices[13] =  0;  indices[14] =  4;
    indices[15] = 11;  indices[16] =  0;  indices[17] = 10;
    indices[18] =  3;  indices[19] = 12;  indices[20] = 13;
    indices[21] =  3;  indices[22] = 13;  indices[23] =  7;
    indices[24] =  7;  indices[25] = 13;  indices[26] =  6;
    indices[27] = 13;  indices[28] = 14;  indices[29] =  6;
    indices[30] = 14;  indices[31] =  2;  indices[32] =  6;
    indices[33] = 15;  indices[34] =  2;  indices[35] = 14;
    indices[36] =  0;  indices[37] =  7;  indices[38] =  4;
    indices[39] =  0;  indices[40] =  3;  indices[41] =  7;
    indices[42] =  2;  indices[43] =  5;  indices[44] =  6;
    indices[45] =  2;  indices[46] =  1;  indices[47] =  5;

    TriMesh* mesh = new0 TriMesh(mPTFormat, vbuffer, ibuffer);
    mesh->SetEffectInstance(mEffect->CreateInstance(wallTexture));
    cube->AttachChild(mesh);

    // portals
    portals = new1<Portal*>(2);

    APoint* modelVertices = new1<APoint>(4);
    modelVertices[0] = vba.Position<Vector3f>(11);
    modelVertices[1] = vba.Position<Vector3f>(10);
    modelVertices[2] = vba.Position<Vector3f>( 9);
    modelVertices[3] = vba.Position<Vector3f>( 8);
    HPlane modelPlane(modelVertices[0], modelVertices[1], modelVertices[2]);
    portals[0] = new0 Portal(4, modelVertices, modelPlane, 0, true);

    modelVertices = new1<APoint>(4);
    modelVertices[0] = vba.Position<Vector3f>(15);
    modelVertices[1] = vba.Position<Vector3f>(14);
    modelVertices[2] = vba.Position<Vector3f>(13);
    modelVertices[3] = vba.Position<Vector3f>(12);
    modelPlane = HPlane(modelVertices[0], modelVertices[1], modelVertices[2]);
    portals[1] = new0 Portal(4, modelVertices, modelPlane, 0, true);

    // floor
    vbuffer = new0 VertexBuffer(4, mPTStride);
    vba.ApplyTo(mPTFormat, vbuffer);
    vba.Position<Vector3f>(0) = Vector3f(-1.0f, -1.0f, 0.0f);
    vba.Position<Vector3f>(1) = Vector3f(+1.0f, -1.0f, 0.0f);
    vba.Position<Vector3f>(2) = Vector3f(+1.0f, +1.0f, 0.0f);
    vba.Position<Vector3f>(3) = Vector3f(-1.0f, +1.0f, 0.0f);
    vba.TCoord<Float2>(0, 0) = Float2(0.0f, 0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(1.0f, 0.0f);
    vba.TCoord<Float2>(0, 2) = Float2(1.0f, 1.0f);
    vba.TCoord<Float2>(0, 3) = Float2(0.0f, 1.0f);

    ibuffer = new0 IndexBuffer(6, sizeof(int));
    indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
    indices[3] = 0;  indices[4] = 2;  indices[5] = 3;

    mesh = new0 TriMesh(mPTFormat, vbuffer, ibuffer);
    mesh->SetEffectInstance(mEffect->CreateInstance(floorTexture));
    cube->AttachChild(mesh);

    // ceiling
    vbuffer = new0 VertexBuffer(4, mPTStride);
    vba.ApplyTo(mPTFormat, vbuffer);
    vba.Position<Vector3f>(0) = Vector3f(-1.0f, -1.0f, 2.0f);
    vba.Position<Vector3f>(1) = Vector3f(+1.0f, -1.0f, 2.0f);
    vba.Position<Vector3f>(2) = Vector3f(+1.0f, +1.0f, 2.0f);
    vba.Position<Vector3f>(3) = Vector3f(-1.0f, +1.0f, 2.0f);
    vba.TCoord<Float2>(0, 0) = Float2(0.0f, 0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(1.0f, 0.0f);
    vba.TCoord<Float2>(0, 2) = Float2(1.0f, 1.0f);
    vba.TCoord<Float2>(0, 3) = Float2(0.0f, 1.0f);

    ibuffer = new0 IndexBuffer(6, sizeof(int));
    indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 2;  indices[2] = 1;
    indices[3] = 0;  indices[4] = 3;  indices[5] = 2;

    mesh = new0 TriMesh(mPTFormat, vbuffer, ibuffer);
    mesh->SetEffectInstance(mEffect->CreateInstance(ceilingTexture));
    cube->AttachChild(mesh);
}
//----------------------------------------------------------------------------
void Portals::CreateEndCube (Texture2D* floorTexture,
    Texture2D* ceilingTexture, Texture2D* wallTexture, Node*& cube,
    Portal**& portals)
{
    cube = new0 Node();

    // walls
    VertexBuffer* vbuffer = new0 VertexBuffer(20, mPTStride);
    VertexBufferAccessor vba(mPTFormat, vbuffer);
    vba.Position<Vector3f>( 0) = Vector3f(-1.0f, -1.0f, 0.0f);
    vba.Position<Vector3f>( 1) = Vector3f(+1.0f, -1.0f, 0.0f);
    vba.Position<Vector3f>( 2) = Vector3f(+1.0f, +1.0f, 0.0f);
    vba.Position<Vector3f>( 3) = Vector3f(-1.0f, +1.0f, 0.0f);
    vba.Position<Vector3f>( 4) = Vector3f(-1.0f, -1.0f, 2.0f);
    vba.Position<Vector3f>( 5) = Vector3f(+1.0f, -1.0f, 2.0f);
    vba.Position<Vector3f>( 6) = Vector3f(+1.0f, +1.0f, 2.0f);
    vba.Position<Vector3f>( 7) = Vector3f(-1.0f, +1.0f, 2.0f);
    vba.Position<Vector3f>( 8) = Vector3f(+0.5f, -1.0f, 0.0f);
    vba.Position<Vector3f>( 9) = Vector3f(+0.5f, -1.0f, 1.5f);
    vba.Position<Vector3f>(10) = Vector3f(-0.5f, -1.0f, 1.5f);
    vba.Position<Vector3f>(11) = Vector3f(-0.5f, -1.0f, 0.0f);
    vba.Position<Vector3f>(12) = Vector3f(-0.5f, +1.0f, 0.0f);
    vba.Position<Vector3f>(13) = Vector3f(-0.5f, +1.0f, 1.5f);
    vba.Position<Vector3f>(14) = Vector3f(+0.5f, +1.0f, 1.5f);
    vba.Position<Vector3f>(15) = Vector3f(+0.5f, +1.0f, 0.0f);
    vba.Position<Vector3f>(16) = Vector3f(+1.0f, +0.5f, 0.0f);
    vba.Position<Vector3f>(17) = Vector3f(+1.0f, +0.5f, 1.5f);
    vba.Position<Vector3f>(18) = Vector3f(+1.0f, -0.5f, 1.5f);
    vba.Position<Vector3f>(19) = Vector3f(+1.0f, -0.5f, 0.0f);
    vba.TCoord<Float2>(0,  0) = Float2(0.00f, 0.00f);
    vba.TCoord<Float2>(0,  1) = Float2(1.00f, 0.00f);
    vba.TCoord<Float2>(0,  2) = Float2(0.00f, 0.00f);
    vba.TCoord<Float2>(0,  3) = Float2(1.00f, 0.00f);
    vba.TCoord<Float2>(0,  4) = Float2(0.00f, 1.00f);
    vba.TCoord<Float2>(0,  5) = Float2(1.00f, 1.00f);
    vba.TCoord<Float2>(0,  6) = Float2(0.00f, 1.00f);
    vba.TCoord<Float2>(0,  7) = Float2(1.00f, 1.00f);
    vba.TCoord<Float2>(0,  8) = Float2(0.75f, 0.00f);
    vba.TCoord<Float2>(0,  9) = Float2(0.75f, 0.75f);
    vba.TCoord<Float2>(0, 10) = Float2(0.25f, 0.75f);
    vba.TCoord<Float2>(0, 11) = Float2(0.25f, 0.00f);
    vba.TCoord<Float2>(0, 12) = Float2(0.75f, 0.00f);
    vba.TCoord<Float2>(0, 13) = Float2(0.75f, 0.75f);
    vba.TCoord<Float2>(0, 14) = Float2(0.25f, 0.75f);
    vba.TCoord<Float2>(0, 15) = Float2(0.25f, 0.00f);
    vba.TCoord<Float2>(0, 16) = Float2(0.25f, 0.00f);
    vba.TCoord<Float2>(0, 17) = Float2(0.25f, 0.75f);
    vba.TCoord<Float2>(0, 18) = Float2(0.75f, 0.75f);
    vba.TCoord<Float2>(0, 19) = Float2(0.75f, 0.00f);

    IndexBuffer* ibuffer = new0 IndexBuffer(60, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[ 0] =  1;  indices[ 1] =  8;  indices[ 2] =  9;
    indices[ 3] =  1;  indices[ 4] =  9;  indices[ 5] =  5;
    indices[ 6] =  5;  indices[ 7] =  9;  indices[ 8] =  4;
    indices[ 9] =  9;  indices[10] = 10;  indices[11] =  4;
    indices[12] = 10;  indices[13] =  0;  indices[14] =  4;
    indices[15] = 11;  indices[16] =  0;  indices[17] = 10;
    indices[18] =  3;  indices[19] = 12;  indices[20] = 13;
    indices[21] =  3;  indices[22] = 13;  indices[23] =  7;
    indices[24] =  7;  indices[25] = 13;  indices[26] =  6;
    indices[27] = 13;  indices[28] = 14;  indices[29] =  6;
    indices[30] = 14;  indices[31] =  2;  indices[32] =  6;
    indices[33] = 15;  indices[34] =  2;  indices[35] = 14;
    indices[36] =  2;  indices[37] = 16;  indices[38] = 17;
    indices[39] =  2;  indices[40] = 17;  indices[41] =  6;
    indices[42] =  6;  indices[43] = 17;  indices[44] =  5;
    indices[45] = 17;  indices[46] = 18;  indices[47] =  5;
    indices[48] = 18;  indices[49] =  1;  indices[50] =  5;
    indices[51] = 19;  indices[52] =  1;  indices[53] = 18;
    indices[54] =  0;  indices[55] =  7;  indices[56] =  4;
    indices[57] =  0;  indices[58] =  3;  indices[59] =  7;

    TriMesh* mesh = new0 TriMesh(mPTFormat, vbuffer, ibuffer);
    mesh->SetEffectInstance(mEffect->CreateInstance(wallTexture));
    cube->AttachChild(mesh);

    // portals
    portals = new1<Portal*>(3);

    APoint* modelVertices = new1<APoint>(4);
    modelVertices[0] = vba.Position<Vector3f>(11);
    modelVertices[1] = vba.Position<Vector3f>(10);
    modelVertices[2] = vba.Position<Vector3f>( 9);
    modelVertices[3] = vba.Position<Vector3f>( 8);
    HPlane modelPlane(modelVertices[0], modelVertices[1], modelVertices[2]);
    portals[0] = new0 Portal(4, modelVertices, modelPlane, 0, true);

    modelVertices = new1<APoint>(4);
    modelVertices[0] = vba.Position<Vector3f>(15);
    modelVertices[1] = vba.Position<Vector3f>(14);
    modelVertices[2] = vba.Position<Vector3f>(13);
    modelVertices[3] = vba.Position<Vector3f>(12);
    modelPlane = HPlane(modelVertices[0], modelVertices[1], modelVertices[2]);
    portals[1] = new0 Portal(4, modelVertices, modelPlane, 0, true);

    modelVertices = new1<APoint>(4);
    modelVertices[0] = vba.Position<Vector3f>(19);
    modelVertices[1] = vba.Position<Vector3f>(18);
    modelVertices[2] = vba.Position<Vector3f>(17);
    modelVertices[3] = vba.Position<Vector3f>(16);
    modelPlane = HPlane(modelVertices[0], modelVertices[1], modelVertices[2]);
    portals[2] = new0 Portal(4, modelVertices, modelPlane, 0, true);

    // floor
    vbuffer = new0 VertexBuffer(4, mPTStride);
    vba.ApplyTo(mPTFormat, vbuffer);
    vba.Position<Vector3f>(0) = Vector3f(-1.0f, -1.0f, 0.0f);
    vba.Position<Vector3f>(1) = Vector3f(+1.0f, -1.0f, 0.0f);
    vba.Position<Vector3f>(2) = Vector3f(+1.0f, +1.0f, 0.0f);
    vba.Position<Vector3f>(3) = Vector3f(-1.0f, +1.0f, 0.0f);
    vba.TCoord<Float2>(0, 0) = Float2(0.0f, 0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(1.0f, 0.0f);
    vba.TCoord<Float2>(0, 2) = Float2(1.0f, 1.0f);
    vba.TCoord<Float2>(0, 3) = Float2(0.0f, 1.0f);

    ibuffer = new0 IndexBuffer(6, sizeof(int));
    indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
    indices[3] = 0;  indices[4] = 2;  indices[5] = 3;

    mesh = new0 TriMesh(mPTFormat, vbuffer, ibuffer);
    mesh->SetEffectInstance(mEffect->CreateInstance(floorTexture));
    cube->AttachChild(mesh);

    // ceiling
    vbuffer = new0 VertexBuffer(4, mPTStride);
    vba.ApplyTo(mPTFormat, vbuffer);
    vba.Position<Vector3f>(0) = Vector3f(-1.0f, -1.0f, 2.0f);
    vba.Position<Vector3f>(1) = Vector3f(+1.0f, -1.0f, 2.0f);
    vba.Position<Vector3f>(2) = Vector3f(+1.0f, +1.0f, 2.0f);
    vba.Position<Vector3f>(3) = Vector3f(-1.0f, +1.0f, 2.0f);
    vba.TCoord<Float2>(0, 0) = Float2(0.0f, 0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(1.0f, 0.0f);
    vba.TCoord<Float2>(0, 2) = Float2(1.0f, 1.0f);
    vba.TCoord<Float2>(0, 3) = Float2(0.0f, 1.0f);

    ibuffer = new0 IndexBuffer(6, sizeof(int));
    indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 2;  indices[2] = 1;
    indices[3] = 0;  indices[4] = 3;  indices[5] = 2;

    mesh = new0 TriMesh(mPTFormat, vbuffer, ibuffer);
    mesh->SetEffectInstance(mEffect->CreateInstance(ceilingTexture));
    cube->AttachChild(mesh);
}
//----------------------------------------------------------------------------
void Portals::CreateDiagonalConnector (Texture2D* floorTexture,
    Texture2D* ceilingTexture, Texture2D* wallTexture, Node*& cube,
    Portal**& portals)
{
    cube = new0 Node();

    // walls
    VertexBuffer* vbuffer = new0 VertexBuffer(16, mPTStride);
    VertexBufferAccessor vba(mPTFormat, vbuffer);
    vba.Position<Vector3f>( 0) = Vector3f(3.0f, 1.0f, 0.0f);
    vba.Position<Vector3f>( 1) = Vector3f(5.0f, 1.0f, 0.0f);
    vba.Position<Vector3f>( 2) = Vector3f(1.0f, 5.0f, 0.0f);
    vba.Position<Vector3f>( 3) = Vector3f(1.0f, 3.0f, 0.0f);
    vba.Position<Vector3f>( 4) = Vector3f(3.0f, 1.0f, 2.0f);
    vba.Position<Vector3f>( 5) = Vector3f(5.0f, 1.0f, 2.0f);
    vba.Position<Vector3f>( 6) = Vector3f(1.0f, 5.0f, 2.0f);
    vba.Position<Vector3f>( 7) = Vector3f(1.0f, 3.0f, 2.0f);
    vba.Position<Vector3f>( 8) = Vector3f(4.5f, 1.0f, 0.0f);
    vba.Position<Vector3f>( 9) = Vector3f(4.5f, 1.0f, 1.5f);
    vba.Position<Vector3f>(10) = Vector3f(3.5f, 1.0f, 1.5f);
    vba.Position<Vector3f>(11) = Vector3f(3.5f, 1.0f, 0.0f);
    vba.Position<Vector3f>(12) = Vector3f(1.0f, 3.5f, 0.0f);
    vba.Position<Vector3f>(13) = Vector3f(1.0f, 3.5f, 1.5f);
    vba.Position<Vector3f>(14) = Vector3f(1.0f, 4.5f, 1.5f);
    vba.Position<Vector3f>(15) = Vector3f(1.0f, 4.5f, 0.0f);
    vba.TCoord<Float2>(0,  0) = Float2(0.00f, 0.00f);
    vba.TCoord<Float2>(0,  1) = Float2(1.00f, 0.00f);
    vba.TCoord<Float2>(0,  2) = Float2(0.00f, 0.00f);
    vba.TCoord<Float2>(0,  3) = Float2(1.00f, 0.00f);
    vba.TCoord<Float2>(0,  4) = Float2(0.00f, 1.00f);
    vba.TCoord<Float2>(0,  5) = Float2(1.00f, 1.00f);
    vba.TCoord<Float2>(0,  6) = Float2(0.00f, 1.00f);
    vba.TCoord<Float2>(0,  7) = Float2(1.00f, 1.00f);
    vba.TCoord<Float2>(0,  8) = Float2(0.75f, 0.00f);
    vba.TCoord<Float2>(0,  9) = Float2(0.75f, 0.75f);
    vba.TCoord<Float2>(0, 10) = Float2(0.25f, 0.75f);
    vba.TCoord<Float2>(0, 11) = Float2(0.25f, 0.00f);
    vba.TCoord<Float2>(0, 12) = Float2(0.75f, 0.00f);
    vba.TCoord<Float2>(0, 13) = Float2(0.75f, 0.75f);
    vba.TCoord<Float2>(0, 14) = Float2(0.25f, 0.75f);
    vba.TCoord<Float2>(0, 15) = Float2(0.25f, 0.00f);

    IndexBuffer* ibuffer = new0 IndexBuffer(48, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[ 0] =  1;  indices[ 1] =  8;  indices[ 2] =  9;
    indices[ 3] =  1;  indices[ 4] =  9;  indices[ 5] =  5;
    indices[ 6] =  5;  indices[ 7] =  9;  indices[ 8] =  4;
    indices[ 9] =  9;  indices[10] = 10;  indices[11] =  4;
    indices[12] = 10;  indices[13] =  0;  indices[14] =  4;
    indices[15] = 11;  indices[16] =  0;  indices[17] = 10;
    indices[18] =  3;  indices[19] = 12;  indices[20] = 13;
    indices[21] =  3;  indices[22] = 13;  indices[23] =  7;
    indices[24] =  7;  indices[25] = 13;  indices[26] =  6;
    indices[27] = 13;  indices[28] = 14;  indices[29] =  6;
    indices[30] = 14;  indices[31] =  2;  indices[32] =  6;
    indices[33] = 15;  indices[34] =  2;  indices[35] = 14;
    indices[36] =  0;  indices[37] =  7;  indices[38] =  4;
    indices[39] =  0;  indices[40] =  3;  indices[41] =  7;
    indices[42] =  2;  indices[43] =  5;  indices[44] =  6;
    indices[45] =  2;  indices[46] =  1;  indices[47] =  5;

    TriMesh* mesh = new0 TriMesh(mPTFormat, vbuffer, ibuffer);
    mesh->SetEffectInstance(mEffect->CreateInstance(wallTexture));
    cube->AttachChild(mesh);

    // portals
    portals = new1<Portal*>(2);

    APoint* modelVertices = new1<APoint>(4);
    modelVertices[0] = vba.Position<Vector3f>(11);
    modelVertices[1] = vba.Position<Vector3f>(10);
    modelVertices[2] = vba.Position<Vector3f>( 9);
    modelVertices[3] = vba.Position<Vector3f>( 8);
    HPlane modelPlane(modelVertices[0], modelVertices[1], modelVertices[2]);
    portals[0] = new0 Portal(4, modelVertices, modelPlane, 0, true);

    modelVertices = new1<APoint>(4);
    modelVertices[0] = vba.Position<Vector3f>(15);
    modelVertices[1] = vba.Position<Vector3f>(14);
    modelVertices[2] = vba.Position<Vector3f>(13);
    modelVertices[3] = vba.Position<Vector3f>(12);
    modelPlane = HPlane(modelVertices[0], modelVertices[1], modelVertices[2]);
    portals[1] = new0 Portal(4, modelVertices, modelPlane, 0, true);

    // floor
    vbuffer = new0 VertexBuffer(4, mPTStride);
    vba.ApplyTo(mPTFormat, vbuffer);
    vba.Position<Vector3f>(0) = Vector3f(3.0f,1.0f,0.0f);
    vba.Position<Vector3f>(1) = Vector3f(5.0f,1.0f,0.0f);
    vba.Position<Vector3f>(2) = Vector3f(1.0f,5.0f,0.0f);
    vba.Position<Vector3f>(3) = Vector3f(1.0f,3.0f,0.0f);
    vba.TCoord<Float2>(0, 0) = Float2(0.0f, 0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(1.0f, 0.0f);
    vba.TCoord<Float2>(0, 2) = Float2(1.0f, 1.0f);
    vba.TCoord<Float2>(0, 3) = Float2(0.0f, 1.0f);

    ibuffer = new0 IndexBuffer(6, sizeof(int));
    indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
    indices[3] = 0;  indices[4] = 2;  indices[5] = 3;

    mesh = new0 TriMesh(mPTFormat, vbuffer, ibuffer);
    mesh->SetEffectInstance(mEffect->CreateInstance(floorTexture));
    cube->AttachChild(mesh);

    // ceiling
    vbuffer = new0 VertexBuffer(4, mPTStride);
    vba.ApplyTo(mPTFormat, vbuffer);
    vba.Position<Vector3f>(0) = Vector3f(3.0f, 1.0f, 2.0f);
    vba.Position<Vector3f>(1) = Vector3f(5.0f, 1.0f, 2.0f);
    vba.Position<Vector3f>(2) = Vector3f(1.0f, 5.0f, 2.0f);
    vba.Position<Vector3f>(3) = Vector3f(1.0f, 3.0f, 2.0f);
    vba.TCoord<Float2>(0, 0) = Float2(0.0f, 0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(1.0f, 0.0f);
    vba.TCoord<Float2>(0, 2) = Float2(1.0f, 1.0f);
    vba.TCoord<Float2>(0, 3) = Float2(0.0f, 1.0f);

    ibuffer = new0 IndexBuffer(6, sizeof(int));
    indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 2;  indices[2] = 1;
    indices[3] = 0;  indices[4] = 3;  indices[5] = 2;

    mesh = new0 TriMesh(mPTFormat, vbuffer, ibuffer);
    mesh->SetEffectInstance(mEffect->CreateInstance(ceilingTexture));
    cube->AttachChild(mesh);
}
//----------------------------------------------------------------------------
