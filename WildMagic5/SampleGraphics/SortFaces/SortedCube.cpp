// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2012/07/04)

#include "SortedCube.h"
#include "Wm5Environment.h"
#include "Wm5Texture2DEffect.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, Node, SortedCube);
WM5_IMPLEMENT_STREAM(SortedCube);
WM5_IMPLEMENT_FACTORY(SortedCube);

//----------------------------------------------------------------------------
SortedCube::SortedCube (Camera* camera,
    const std::string& xpName, const std::string& xmName,
    const std::string& ypName, const std::string& ymName,
    const std::string& zpName, const std::string& zmName)
    :
    mCamera(camera),
    mBackFacing(0)
{
    // Texture effect shared by all faces.
    Texture2DEffect* effect = new0 Texture2DEffect(Shader::SF_LINEAR);
    Texture2D* texture;

    // Child nodes have textures with some alpha values smaller than 1.
    AlphaState* astate = effect->GetAlphaState(0, 0);
    astate->BlendEnabled = true;

    // No culling (all faces drawn).
    CullState* cstate = effect->GetCullState(0, 0);
    cstate->Enabled = false;

    // Depth buffering writes, but no reads (for transparency).
    DepthState* dstate = effect->GetDepthState(0, 0);
    dstate->Enabled = false;
    dstate->Writable = true;

    // All faces share the same vertex and index buffers.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);
    int vstride = vformat->GetStride();

    VertexBuffer* vbuffer = new0 VertexBuffer(4, vstride);

    VertexBufferAccessor vba(vformat, vbuffer);
    vba.Position<Float3>(0) = Float3(-1.0f, -1.0f, 0.0f);
    vba.Position<Float3>(1) = Float3( 1.0f, -1.0f, 0.0f);
    vba.Position<Float3>(2) = Float3( 1.0f,  1.0f, 0.0f);
    vba.Position<Float3>(3) = Float3(-1.0f,  1.0f, 0.0f);
    vba.TCoord<Float2>(0, 0) = Float2(0.0f, 0.0f);
    vba.TCoord<Float2>(0, 1) = Float2(1.0f, 0.0f);
    vba.TCoord<Float2>(0, 2) = Float2(1.0f, 1.0f);
    vba.TCoord<Float2>(0, 3) = Float2(0.0f, 1.0f);

    IndexBuffer* ibuffer = new0 IndexBuffer(6, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    indices[0] = 0;  indices[1] = 1;  indices[2] = 2;
    indices[3] = 0;  indices[4] = 2;  indices[5] = 3;

    // The six TriMesh children of this object are initially stored in the
    // order: x=1 face, x=-1 face, y=1 face, y=-1 face, z=1 face, z=-1 face.

    // xp face (x = 1)
    mFace[0] = new0 TriMesh(vformat, vbuffer, ibuffer);
    mFace[0]->LocalTransform.SetRotate(HMatrix(AVector::UNIT_Y,
        Mathf::HALF_PI));
    mFace[0]->LocalTransform.SetTranslate(APoint(1.0f, 0.0f, 0.0f));
    texture = Texture2D::LoadWMTF(Environment::GetPathR(xpName));
    mFace[0]->SetEffectInstance(effect->CreateInstance(texture));
    AttachChild(mFace[0]);

    // xm face (x = -1)
    mFace[1] = new0 TriMesh(vformat, vbuffer, ibuffer);
    mFace[1]->LocalTransform.SetRotate(HMatrix(AVector::UNIT_Y,
        -Mathf::HALF_PI));
    mFace[1]->LocalTransform.SetTranslate(APoint(-1.0f, 0.0f, 0.0f));
    texture = Texture2D::LoadWMTF(Environment::GetPathR(xmName));
    mFace[1]->SetEffectInstance(effect->CreateInstance(texture));
    AttachChild(mFace[1]);

    // yp face (y = 1)
    mFace[2] = new0 TriMesh(vformat, vbuffer, ibuffer);
    mFace[2]->LocalTransform.SetRotate(HMatrix(AVector::UNIT_X,
        -Mathf::HALF_PI));
    mFace[2]->LocalTransform.SetTranslate(APoint(0.0f, 1.0f, 0.0f));
    texture = Texture2D::LoadWMTF(Environment::GetPathR(ypName));
    mFace[2]->SetEffectInstance(effect->CreateInstance(texture));
    AttachChild(mFace[2]);

    // ym face (y = -1)
    mFace[3] = new0 TriMesh(vformat, vbuffer, ibuffer);
    mFace[3]->LocalTransform.SetRotate(HMatrix(AVector::UNIT_X,
        Mathf::HALF_PI));
    mFace[3]->LocalTransform.SetTranslate(APoint(0.0f, -1.0f, 0.0f));
    texture = Texture2D::LoadWMTF(Environment::GetPathR(ymName));
    mFace[3]->SetEffectInstance(effect->CreateInstance(texture));
    AttachChild(mFace[3]);

    // zp face (z = 1)
    mFace[4] = new0 TriMesh(vformat, vbuffer, ibuffer);
    mFace[4]->LocalTransform.SetTranslate(APoint(0.0f, 0.0f, 1.0f));
    texture = Texture2D::LoadWMTF(Environment::GetPathR(zpName));
    mFace[4]->SetEffectInstance(effect->CreateInstance(texture));
    AttachChild(mFace[4]);

    // zm face (z = -1)
    mFace[5] = new0 TriMesh(vformat, vbuffer, ibuffer);
    mFace[5]->LocalTransform.SetRotate(HMatrix(AVector::UNIT_Y,
        Mathf::PI));
    mFace[5]->LocalTransform.SetTranslate(APoint(0.0f, 0.0f, -1.0f));
    texture = Texture2D::LoadWMTF(Environment::GetPathR(zmName));
    mFace[5]->SetEffectInstance(effect->CreateInstance(texture));
    AttachChild(mFace[5]);
}
//----------------------------------------------------------------------------
SortedCube::~SortedCube ()
{
}
//----------------------------------------------------------------------------
void SortedCube::SetCamera (Camera* camera)
{
    mCamera = camera;
}
//----------------------------------------------------------------------------
void SortedCube::SortFaces ()
{
    // Must have a camera for sorting.
    if (!mCamera)
    {
        return;
    }

    // Inverse transform the camera world view direction into the model space
    // of the cube.
    AVector modelDVector = WorldTransform.Inverse()*mCamera->GetDVector();

    // Test for back faces.
    int newBackFacing = 0, numBackFacing = 0;

    // Test xp face.
    if (modelDVector.X() > 0.0f)
    {
        newBackFacing |=  1;
        ++numBackFacing;
    }

    // Test xm face.
    if (modelDVector.X() < 0.0f)
    {
        newBackFacing |=  2;
        ++numBackFacing;
    }

    // Test yp face.
    if (modelDVector.Y() > 0.0f)
    {
        newBackFacing |=  4;
        ++numBackFacing;
    }

    // Test ym face.
    if (modelDVector.Y() < 0.0f)
    {
        newBackFacing |=  8;
        ++numBackFacing;
    }

    // Test zp face.
    if (modelDVector.Z() > 0.0f)
    {
        newBackFacing |= 16;
        ++numBackFacing;
    }

    // Test zm face.
    if (modelDVector.Z() < 0.0f)
    {
        newBackFacing |= 32;
        ++numBackFacing;
    }

    if (newBackFacing != mBackFacing)
    {
        // Resorting is needed.  Reassign the sorted children to the node
        // parent.  Back-facing children go first, front-facing second.
        mBackFacing = newBackFacing;

        // Detach old children.
        int i;
        for (i = 0; i < 6; ++i)
        {
            SetChild(i, 0);
        }

        // Attach new children.
        int bStart = 0, fStart = numBackFacing, mask;
        for (i = 0, mask = 1; i < 6; ++i, mask <<= 1)
        {
            if (mBackFacing & mask)
            {
                SetChild(bStart++, mFace[i]);
            }
            else
            {
                SetChild(fStart++, mFace[i]);
            }
        }
    }
}
//----------------------------------------------------------------------------
void SortedCube::GetVisibleSet (Culler& culler, bool noCull)
{
    SortFaces();
    Node::GetVisibleSet(culler, noCull);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Name support.
//----------------------------------------------------------------------------
Object* SortedCube::GetObjectByName (const std::string& name)
{
    Object* found = Node::GetObjectByName(name);
    if (found)
    {
        return found;
    }

    WM5_GET_OBJECT_BY_NAME(mCamera, name, found);
    return 0;
}
//----------------------------------------------------------------------------
void SortedCube::GetAllObjectsByName (const std::string& name,
    std::vector<Object*>& objects)
{
    Node::GetAllObjectsByName(name, objects);

    WM5_GET_ALL_OBJECTS_BY_NAME(mCamera, name, objects);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Streaming support.
//----------------------------------------------------------------------------
SortedCube::SortedCube (LoadConstructor)
    :
    mCamera(0),
    mBackFacing(0)
{
}
//----------------------------------------------------------------------------
void SortedCube::Load (InStream& source)
{
    WM5_BEGIN_DEBUG_STREAM_LOAD(source);

    Node::Load(source);

    source.ReadPointer(mCamera);

    WM5_END_DEBUG_STREAM_LOAD(SortedCube, source);
}
//----------------------------------------------------------------------------
void SortedCube::Link (InStream& source)
{
    Node::Link(source);

    source.ResolveLink(mCamera);
}
//----------------------------------------------------------------------------
void SortedCube::PostLink ()
{
    Node::PostLink();
}
//----------------------------------------------------------------------------
bool SortedCube::Register (OutStream& target) const
{
    if (Node::Register(target))
    {
        target.Register(mCamera);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void SortedCube::Save (OutStream& target) const
{
    WM5_BEGIN_DEBUG_STREAM_SAVE(target);

    Node::Save(target);

    target.WritePointer(mCamera);

    WM5_END_DEBUG_STREAM_SAVE(SortedCube, target);
}
//----------------------------------------------------------------------------
int SortedCube::GetStreamingSize () const
{
    int size = Node::GetStreamingSize();
    size += WM5_POINTERSIZE(mCamera);
    return size;
}
//----------------------------------------------------------------------------
