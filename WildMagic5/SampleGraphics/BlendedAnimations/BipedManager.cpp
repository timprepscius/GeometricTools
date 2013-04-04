// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.1.0 (2010/04/14)

#include "BipedManager.h"

//----------------------------------------------------------------------------
BipedManager::BipedManager (const std::string& rootPath,
    const std::string& name)
{
    // Vertex format shared by triangle meshes.
    VertexFormat* vformat = VertexFormat::Create(3,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_NORMAL, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);

    // Textures shared by triangle meshes.
    Texture2DEffect* effect = new0 Texture2DEffect(Shader::SF_LINEAR_LINEAR,
        Shader::SC_REPEAT, Shader::SC_REPEAT);
    std::string textureName = rootPath + "Skins/Skins0.texture.wmtf";
    Texture2D* texture0 = Texture2D::LoadWMTF(textureName);
    textureName = rootPath + "Skins/Skins1.texture.wmtf";
    Texture2D* texture1 = Texture2D::LoadWMTF(textureName);
    VisualEffectInstance* instance[2] =
    {
        effect->CreateInstance(texture0),
        effect->CreateInstance(texture1)
    };

    PreSpatialArray preSpatialArray;
    PreSkinArray preSkinArray;
    SpatialMap spatialMap;

    std::string filename = rootPath + name + ".txt";
    std::ifstream inFile(filename.c_str());
    while (!inFile.eof())
    {
        std::string line;
        getline(inFile, line);
        if (line == "")
        {
            // The file contains no blank lines, except for the very last one.
            break;
        }

        // Strip off initial white space.
        std::string::size_type begin = line.find_first_not_of(" ");
        if (begin > 0)
        {
            line = line.substr(begin);
        }

        std::string::size_type end;
        std::string name;
        if (line.find("Node") == 0)
        {
            // Load the node.
            begin = line.find("<");
            end = line.find(">");
            name = line.substr(begin + 1, end - 1 - begin);
            PreSpatial* preSpatial = LoadNode(rootPath, name);
            preSpatialArray.push_back(preSpatial);
            Node* node = (Node*)preSpatial->Associate;
            spatialMap[name] = node;

            // Load the transform controllers.
            NodeCtrl nc;
            nc.first = node;

            nc.second = LoadTransformController(rootPath, name, "Idle");
            mIdleArray.push_back(nc);

            nc.second = LoadTransformController(rootPath, name, "Walk");
            mWalkArray.push_back(nc);

            nc.second = LoadTransformController(rootPath, name, "Run");
            mRunArray.push_back(nc);
        }
        else if (line.find("TriMesh") == 0)
        {
            // Load the mesh.
            begin = line.find("<");
            end = line.find(">");
            name = line.substr(begin + 1, end - 1 - begin);
            int suffix = name[name.length()-1] - '0';
            PreSpatial* preSpatial = LoadMesh(rootPath, name, vformat,
                instance[suffix]);
            preSpatialArray.push_back(preSpatial);
            spatialMap[name] = preSpatial->Associate;

            // Load the skin controller.
            PreSkin* preSkin = LoadSkinController(rootPath, name);
            preSkinArray.push_back(preSkin);

            // Attach the skin controller to the mesh.
            preSpatial->Associate->AttachController(preSkin->Associate);
        }
    }

    // Resolve the bone links.
    PreSkinArray::iterator skiter = preSkinArray.begin();
    PreSkinArray::iterator skend = preSkinArray.end();
    for (/**/; skiter != skend; ++skiter)
    {
        PreSkin* preSkin = *skiter;
        SkinController* ctrl = preSkin->Associate;
        Node** bones = ctrl->GetBones();
        StringArray::iterator striter = preSkin->BoneNames.begin();
        StringArray::iterator strend = preSkin->BoneNames.end();
        for (int i = 0; striter != strend; ++striter, ++i)
        {
            std::string boneName = *striter;
            SpatialMap::iterator iter = spatialMap.find(boneName);
            bones[i] = DynamicCast<Node>(iter->second);
        }
    }

    // Assemble the biped hierarchy.
    PreSpatialArray::iterator spiter = preSpatialArray.begin();
    PreSpatialArray::iterator spend = preSpatialArray.end();
    for (/**/; spiter != spend; ++spiter)
    {
        PreSpatial* preSpatial = *spiter;
        Spatial* spatial = preSpatial->Associate;
        std::string name = spatial->GetName();
        Node* node = DynamicCast<Node>(spatial);
        if (node)
        {
            StringArray::iterator striter = preSpatial->ChildNames.begin();
            StringArray::iterator strend = preSpatial->ChildNames.end();
            for (/**/; striter != strend; ++striter)
            {
                std::string childName = *striter;
                SpatialMap::iterator siter = spatialMap.find(childName);
                node->AttachChild(siter->second);
            }
        }
    }

    mRoot = StaticCast<Node>(preSpatialArray[0]->Associate);

    spiter = preSpatialArray.begin();
    spend = preSpatialArray.end();
    for (/**/; spiter != spend; ++spiter)
    {
        PreSpatial* preSpatial = *spiter;
        delete0(preSpatial);
    }

    skiter = preSkinArray.begin();
    skend = preSkinArray.end();
    for (/**/; skiter != skend; ++skiter)
    {
        PreSkin* preSkin = *skiter;
        delete0(preSkin);
    }

    // Create the blend controllers.
    const int numControllers = (int)mIdleArray.size();
    mIdleWalkArray.resize(numControllers);
    mWalkRunArray.resize(numControllers);
    for (int i = 0; i < numControllers; ++i)
    {
        const NodeCtrl& nc0 = mIdleArray[i];
        const NodeCtrl& nc1 = mWalkArray[i];
        const NodeCtrl& nc2 = mRunArray[i];

        mIdleWalkArray[i].first = nc0.first;
        mIdleWalkArray[i].second = new0 BlendTransformController(nc0.second,
            nc1.second, true, true, false);

        mWalkRunArray[i].first = nc0.first;
        mWalkRunArray[i].second = new0 BlendTransformController(nc1.second,
            nc2.second, true, true, false);
    }
}
//----------------------------------------------------------------------------
BipedManager::~BipedManager ()
{
}
//----------------------------------------------------------------------------
void BipedManager::Initialize (int idleWalkCount, int walkCount,
    int walkRunCount)
{
    mState = ANIM_IDLE;
    mCount = 0;
    mCountMax[ANIM_IDLE] = 0;
    mCountMax[ANIM_IDLE_WALK] = idleWalkCount;
    mCountMax[ANIM_WALK] = mCountMax[ANIM_IDLE_WALK] + walkCount;
    mCountMax[ANIM_WALK_RUN] = mCountMax[ANIM_WALK] + walkRunCount;
    mCountMax[ANIM_RUN] = mCountMax[ANIM_WALK_RUN];
    mWeight = 0.0f;
    mDeltaWeight0 = 1.0f/(float)idleWalkCount;
    mDeltaWeight1 = 1.0f/(float)walkRunCount;

    DoIdle();
}
//----------------------------------------------------------------------------
void BipedManager::Update (bool blendIdleToWalk, bool blendWalkToRun)
{
    if (blendIdleToWalk)
    {
        if (mState == ANIM_IDLE)
        {
            TransitionIdleToIdleWalk();
            mCount = 1;
            return;
        }

        if (mState == ANIM_IDLE_WALK)
        {
            if (mCount++ < mCountMax[ANIM_IDLE_WALK])
            {
                ContinueIdleWalk();
            }
            else
            {
                TransitionIdleWalkToWalk();
            }
            return;
        }

        if (mState == ANIM_WALK)
        {
            if (blendWalkToRun)
            {
                if (mCount++ == mCountMax[ANIM_WALK])
                {
                    TransitionWalkToWalkRun();
                }
            }
            else
            {
                // continue walk
            }
            return;
        }

        if (mState == ANIM_WALK_RUN)
        {
            if (blendWalkToRun)
            {
                if (mCount++ < mCountMax[ANIM_WALK_RUN])
                {
                    ContinueWalkRun();
                }
                else
                {
                    TransitionWalkRunToRun();
                }
            }
            else
            {
                if (--mCount > mCountMax[ANIM_WALK])
                {
                    ContinueRunWalk();
                }
                else
                {
                    TransitionRunWalkToWalk();
                }
            }
            return;
        }

        if (mState == ANIM_RUN)
        {
            if (blendWalkToRun)
            {
                // continue run
            }
            else
            {
                --mCount;
                TransitionRunToRunWalk();
            }
            return;
        }
    }
    else
    {
        if (mState == ANIM_RUN)
        {
            --mCount;
            TransitionRunToRunWalk();
            return;
        }

        if (mState == ANIM_WALK_RUN)
        {
            if (--mCount > mCountMax[ANIM_WALK])
            {
                ContinueRunWalk();
            }
            else
            {
                TransitionRunWalkToWalk();
            }
            return;
        }

        if (mState == ANIM_WALK)
        {
            if (--mCount == mCountMax[ANIM_IDLE_WALK])
            {
                TransitionWalkToWalkIdle();
            }
            else
            {
                // continue walk
            }
            return;
        }

        if (mState == ANIM_IDLE_WALK)
        {
            if (--mCount > mCountMax[ANIM_IDLE])
            {
                ContinueWalkIdle();
            }
            else
            {
                TransitionWalkIdleToIdle();
            }
            return;
        }

        if (mState == ANIM_IDLE)
        {
            // continue idle
            return;
        }
    }
}
//----------------------------------------------------------------------------
float BipedManager::GetSpeed () const
{
    return ((float)mCount)/(float)mCountMax[ANIM_RUN];
}
//----------------------------------------------------------------------------
BipedManager::PreSpatial* BipedManager::LoadNode (const std::string& rootPath,
    const std::string& name)
{
    PreSpatial* preSpatial = new0 PreSpatial();
    Node* node = new0 Node();
    node->SetName(name);
    preSpatial->Associate = node;

    std::string filename = rootPath + "Bones/" + name + ".node.raw";
    FileIO inFile(filename, FileIO::FM_DEFAULT_READ);

    int numChildren;
    inFile.Read(sizeof(int), &numChildren);
    for (int i = 0; i < numChildren; ++i)
    {
        int length;
        inFile.Read(sizeof(int), &length);
        char* text = new1<char>(length + 1);
        inFile.Read(sizeof(char), length, text);
        text[length] = 0;
        preSpatial->ChildNames.push_back(std::string(text));
        delete1(text);
    }

    inFile.Close();
    return preSpatial;
}
//----------------------------------------------------------------------------
BipedManager::PreSpatial* BipedManager::LoadMesh (const std::string& rootPath,
    const std::string& name, VertexFormat* vformat,
    VisualEffectInstance* instance)
{
    std::string filename = rootPath + "Skins/" + name + ".mesh.raw";
    FileIO inFile(filename, FileIO::FM_DEFAULT_READ);

    int numVertices;
    inFile.Read(sizeof(int), &numVertices);
    int vstride = vformat->GetStride();
    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, vstride);
    inFile.Read(sizeof(float), 8*numVertices, vbuffer->GetData());

    int numIndices;
    inFile.Read(sizeof(int), &numIndices);
    IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    inFile.Read(sizeof(int), numIndices, indices);

    inFile.Close();

    TriMesh* mesh = new0 TriMesh(vformat, vbuffer, ibuffer);
    mesh->SetName(name);
    mesh->SetEffectInstance(instance);

    PreSpatial* preSpatial = new0 PreSpatial();
    preSpatial->Associate = mesh;
    return preSpatial;
}
//----------------------------------------------------------------------------
BipedManager::PreSkin* BipedManager::LoadSkinController (
    const std::string& rootPath, const std::string& name)
{
    std::string filename = rootPath + "Skins/" + name + ".skinctrl.raw";
    FileIO inFile(filename, FileIO::FM_DEFAULT_READ);

    int repeatType, active;
    double minTime, maxTime, phase, frequency;
    inFile.Read(sizeof(int), &repeatType);
    inFile.Read(sizeof(double), &minTime);
    inFile.Read(sizeof(double), &maxTime);
    inFile.Read(sizeof(double), &phase);
    inFile.Read(sizeof(double), &frequency);
    inFile.Read(sizeof(int), &active);

    int numVertices, numBones;
    inFile.Read(sizeof(int), &numVertices);
    inFile.Read(sizeof(int), &numBones);

    SkinController* ctrl = new0 SkinController(numVertices, numBones);
    ctrl->SetName(name);
    PreSkin* preSkin = new0 PreSkin();
    preSkin->Associate = ctrl;

    ctrl->Repeat = (Controller::RepeatType)repeatType;
    ctrl->MinTime = minTime;
    ctrl->MaxTime = maxTime;
    ctrl->Phase = phase;
    ctrl->Frequency = frequency;
    ctrl->Active = (active > 0);

    int b, v;
    for (b = 0; b < numBones; ++b)
    {
        int length;
        inFile.Read(sizeof(int), &length);
        char* text = new1<char>(length + 1);
        inFile.Read(sizeof(char), length, text);
        text[length] = 0;
        preSkin->BoneNames.push_back(std::string(text));
        delete1(text);
    }

    float** weights = ctrl->GetWeights();
    APoint** offsets = ctrl->GetOffsets();

    for (v = 0; v < numVertices; ++v)
    {
        for (b = 0; b < numBones; ++b)
        {
            float weight;
            inFile.Read(sizeof(float), &weight);
            weights[v][b] = weight;
        }
    }

    for (v = 0; v < numVertices; ++v)
    {
        for (b = 0; b < numBones; ++b)
        {
            Float3 offset;
            inFile.Read(sizeof(float), 3, &offset);
            for (int j = 0; j < 3; ++j)
            {
                offsets[v][b][j] = offset[j];
            }
        }
    }

    inFile.Close();

    return preSkin;
}
//----------------------------------------------------------------------------
TransformController* BipedManager::LoadTransformController (
    const std::string& rootPath, const std::string& name,
    const std::string& animation)
{
    std::string filename = rootPath + "Animations/" + animation + "/" +
        name + ".xfrmctrl.raw";
    FileIO inFile(filename, FileIO::FM_DEFAULT_READ);

    int isKeyframeController;
    inFile.Read(sizeof(int), &isKeyframeController);

    TransformController* ctrl;
    int repeatType, active;
    double minTime, maxTime, phase, frequency;
    inFile.Read(sizeof(int), &repeatType);
    inFile.Read(sizeof(double), &minTime);
    inFile.Read(sizeof(double), &maxTime);
    inFile.Read(sizeof(double), &phase);
    inFile.Read(sizeof(double), &frequency);
    inFile.Read(sizeof(int), &active);

    float mat[9], trn[3], sca[3];
    char isIdentity, isRSMatrix, isUniformScale, dummy;
    inFile.Read(sizeof(float), 9, mat);
    inFile.Read(sizeof(float), 3, trn);
    inFile.Read(sizeof(float), 3, sca);
    inFile.Read(sizeof(char), &isIdentity);
    inFile.Read(sizeof(char), &isRSMatrix);
    inFile.Read(sizeof(char), &isUniformScale);
    inFile.Read(sizeof(char), &dummy);

    Transform localTransform;
    localTransform.SetTranslate(APoint(trn[0], trn[1], trn[2]));
    if (isUniformScale)
    {
        localTransform.SetUniformScale(sca[0]);
    }
    else
    {
        localTransform.SetScale(APoint(sca[0], sca[1], sca[2]));
    }
    if (isRSMatrix)
    {
        localTransform.SetRotate(HMatrix(
            mat[0], mat[1], mat[2], 0.0f,
            mat[3], mat[4], mat[5], 0.0f,
            mat[6], mat[7], mat[8], 0.0f,
            0.0f,   0.0f,   0.0f,   1.0f));
    }
    else
    {
        localTransform.SetMatrix(HMatrix(
            mat[0], mat[1], mat[2], 0.0f,
            mat[3], mat[4], mat[5], 0.0f,
            mat[6], mat[7], mat[8], 0.0f,
            0.0f,   0.0f,   0.0f,   1.0f));
    }

    if (isKeyframeController)
    {
        int numTranslations, numRotations, numScales;
        inFile.Read(sizeof(int), &numTranslations);
        inFile.Read(sizeof(int), &numRotations);
        inFile.Read(sizeof(int), &numScales);

        KeyframeController* keyfctrl = new0 KeyframeController(0,
            numTranslations, numRotations, numScales, localTransform);

        if (numTranslations > 0)
        {
            inFile.Read(sizeof(float), numTranslations,
                keyfctrl->GetTranslationTimes());

            inFile.Read(sizeof(float), 4*numTranslations,
                keyfctrl->GetTranslations());
        }

        if (numRotations > 0)
        {
            inFile.Read(sizeof(float), numRotations,
                keyfctrl->GetRotationTimes());

            inFile.Read(sizeof(float), 4*numRotations,
                keyfctrl->GetRotations());
        }

        if (numScales > 0)
        {
            inFile.Read(sizeof(float), numScales,
                keyfctrl->GetScaleTimes());

            inFile.Read(sizeof(float), numScales,
                keyfctrl->GetScales());
        }

        ctrl = keyfctrl;
    }
    else
    {
        // The adjustment to the "Biped" root node is an attempt to get the
        // biped to idle/walk/run on a floor at height zero.  The biped model
        // should have minimally been created so that the foot steps occur at
        // the same height for all animations.
        if (name == "Biped")
        {
            APoint trn = localTransform.GetTranslate();
            if (animation == "Idle")
            {
                trn[2] += 3.5f;
            }
            else if (animation == "Walk")
            {
                trn[2] += 2.0f;
            }
            else // animation == "Run"
            {
                trn[2] += 2.5f;
            }
            localTransform.SetTranslate(trn);
        }

        TransformController* xfrmctrl =
            new0 TransformController(localTransform);

        ctrl = xfrmctrl;
    }

    ctrl->SetName(name);
    ctrl->Repeat = Controller::RT_WRAP;
    ctrl->MinTime = minTime;
    ctrl->MaxTime = maxTime;
    ctrl->Phase = phase;
    ctrl->Frequency = frequency;
    ctrl->Active = (active > 0);

    inFile.Close();
    return ctrl;
}
//----------------------------------------------------------------------------
void BipedManager::GetAnimation (const NodeCtrlArray& ncArray,
    double& minTime, double& maxTime) const
{
    minTime = Mathd::MAX_REAL;
    maxTime = -Mathd::MAX_REAL;
    NodeCtrlArray::const_iterator nciter = ncArray.begin();
    NodeCtrlArray::const_iterator ncend = ncArray.end();
    for (/**/; nciter != ncend; ++nciter)
    {
        TransformController* ctrl = nciter->second;
        if (ctrl->MinTime < minTime)
        {
            minTime = ctrl->MinTime;
        }
        else if (ctrl->MaxTime > maxTime)
        {
            maxTime = ctrl->MaxTime;
        }
    }
}
//----------------------------------------------------------------------------
void BipedManager::SetAnimation (NodeCtrlArray& ncArray, double frequency,
    double phase)
{
    NodeCtrlArray::iterator nciter = ncArray.begin();
    NodeCtrlArray::iterator ncend = ncArray.end();
    for (/**/; nciter != ncend; ++nciter)
    {
        TransformController* ctrl = nciter->second;
        ctrl->Frequency = frequency;
        ctrl->Phase = phase;
    }
}
//----------------------------------------------------------------------------
void BipedManager::SetBlendAnimation (NodeCtrlArray& ncArray, float weight)
{
    NodeCtrlArray::iterator nciter = ncArray.begin();
    NodeCtrlArray::iterator ncend = ncArray.end();
    for (/**/; nciter != ncend; ++nciter)
    {
        BlendTransformController* ctrl =
            StaticCast<BlendTransformController>(nciter->second);

        ctrl->SetWeight(weight);
    }
}
//----------------------------------------------------------------------------
void BipedManager::DoAnimation (NodeCtrlArray& ncArray)
{
    NodeCtrlArray::iterator nciter = ncArray.begin();
    NodeCtrlArray::iterator ncend = ncArray.end();
    for (/**/; nciter != ncend; ++nciter)
    {
        Node* node = nciter->first;
        TransformController* ctrl = nciter->second;
        ctrl->Repeat = Controller::RT_WRAP;
        node->DetachAllControllers();
        node->AttachController(ctrl);
    }
}
//----------------------------------------------------------------------------
void BipedManager::ContinueIdleWalk ()
{
    SetIdleWalk(mWeight);
    mWeight += mDeltaWeight0;
    if (mWeight > 1.0f)
    {
        mWeight = 1.0f;
    }
}
//----------------------------------------------------------------------------
void BipedManager::ContinueWalkRun ()
{
    SetWalkRun(mWeight);
    mWeight += mDeltaWeight1;
    if (mWeight > 1.0f)
    {
        mWeight = 1.0f;
    }
}
//----------------------------------------------------------------------------
void BipedManager::ContinueRunWalk ()
{
    SetWalkRun(mWeight);
    mWeight -= mDeltaWeight1;
    if (mWeight < 0.0f)
    {
        mWeight = 0.0f;
    }
}
//----------------------------------------------------------------------------
void BipedManager::ContinueWalkIdle ()
{
    SetIdleWalk(mWeight);
    mWeight -= mDeltaWeight0;
    if (mWeight < 0.0f)
    {
        mWeight = 0.0f;
    }
}
//----------------------------------------------------------------------------
void BipedManager::TransitionIdleToIdleWalk ()
{
    mState = ANIM_IDLE_WALK;
    DoIdleWalk();
    SetIdleWalk(0.0f);
    mWeight = mDeltaWeight0;
}
//----------------------------------------------------------------------------
void BipedManager::TransitionIdleWalkToWalk ()
{
    mState = ANIM_WALK;
    DoWalk();
}
//----------------------------------------------------------------------------
void BipedManager::TransitionWalkToWalkRun ()
{
    mState = ANIM_WALK_RUN;
    DoWalkRun();
    SetWalkRun(0.0f);
    mWeight = mDeltaWeight1;
}
//----------------------------------------------------------------------------
void BipedManager::TransitionWalkRunToRun ()
{
    mState = ANIM_RUN;
    DoRun();
}
//----------------------------------------------------------------------------
void BipedManager::TransitionRunToRunWalk ()
{
    mState = ANIM_WALK_RUN;
    DoWalkRun();
    SetWalkRun(1.0f);
    mWeight = 1.0f - mDeltaWeight1;
}
//----------------------------------------------------------------------------
void BipedManager::TransitionRunWalkToWalk ()
{
    mState = ANIM_WALK;
    DoWalk();
}
//----------------------------------------------------------------------------
void BipedManager::TransitionWalkToWalkIdle ()
{
    mState = ANIM_IDLE_WALK;
    DoIdleWalk();
    SetIdleWalk(1.0f);
    mWeight = 1.0f - mDeltaWeight0;
}
//----------------------------------------------------------------------------
void BipedManager::TransitionWalkIdleToIdle ()
{
    mState = ANIM_IDLE;
    DoIdle();
}
//----------------------------------------------------------------------------
