// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2012/07/07)

#include "DeformableBall.h"
#include "Wm5Environment.h"
#include "Wm5ExtractSurfaceCubes.h"
#include "Wm5Texture2DEffect.h"
#include "Wm5VertexBufferAccessor.h"
using namespace Wm5;

//----------------------------------------------------------------------------
DeformableBall::DeformableBall (float duration, float period)
{
    Set(duration, period);
    mDeforming = false;
    mDoAffine = true;
    CreateBall();
}
//----------------------------------------------------------------------------
DeformableBall::~DeformableBall ()
{
    delete1(mNormal);
    delete1(mMean);
    delete1(mNeighborCount);
}
//----------------------------------------------------------------------------
void DeformableBall::Set (float duration, float period)
{
    mDuration = duration;
    mDeformMult = 4.0f/(mDuration*mDuration);
    mPeriod = period;
    mMinActive = 0.5f*(mPeriod - mDuration);
    mMaxActive = 0.5f*(mPeriod + mDuration);
    mInvActiveRange = 1.0f/(mMaxActive - mMinActive);
}
//----------------------------------------------------------------------------
void DeformableBall::CreateBall ()
{
    // Create initial image for surface extraction (16 x 16 x 16).
    const int bound = 16;
    float invBoundM1 = 1.0f/(bound - 1);
    int* data = new1<int>(bound*bound*bound);
    ExtractSurfaceCubes extractor(bound, bound, bound, data);

    // Scale function values to [-1024,1024].
    const float imageScale = 1024.0f;

    // Initialize image and extract level surface F = 0.  Data stores samples
    // for (x,y,z) in [-1,1]x[-1,1]x[0,2].
    AVector position;
    position[3] = 0.0f;
    int i = 0;
    for (int z = 0; z < bound; ++z)
    {
        position[2] = -0.1f + 2.2f*invBoundM1*z;
        for (int y = 0; y < bound; ++y)
        {
            position[1] = -1.1f + 2.2f*invBoundM1*y;
            for (int x = 0; x < bound; ++x, ++i)
            {
                position[0] = -1.1f + 2.2f*invBoundM1*x;

                float function;
                AVector gradient;
                ComputeFunction(position, 0.0f, function, gradient);
                data[i] = (int)(imageScale*function);
            }
        }
    }

    // Extract the level surface.
    std::vector<Vector3f> vertices;
    std::vector<TriangleKey> triangles;
    extractor.ExtractContour(0.0f, vertices, triangles);
    extractor.MakeUnique(vertices, triangles);
    extractor.OrientTriangles(vertices, triangles, true);
    delete1(data);

    // Convert to TriMesh object.  Keep track of the level value of the
    // vertices.  Since a vertex might not be exactly on the level surface,
    // we will use
    //     e = max{|F(x,y,z)| : (x,y,z) is a vertex}
    // as the error tolerance for Newton's method in the level surface
    // evolution.
    VertexFormat* vformat = VertexFormat::Create(2,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);
    int vstride = vformat->GetStride();

    int numVertices = (int)vertices.size();
    VertexBuffer* vbuffer = new VertexBuffer(numVertices, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);
    float maxLevel = 0.0f;
    for (i = 0; i < numVertices; ++i)
    {
        Vector3f vertex = vertices[i];
        Vector3f& position = vba.Position<Vector3f>(i);
        position[0] = -1.1f + 2.2f*invBoundM1*vertex[0];
        position[1] = -1.1f + 2.2f*invBoundM1*vertex[1];
        position[2] = -0.1f + 2.2f*invBoundM1*vertex[2];

        float level = position.SquaredLength() - 2.0f*position[2];
        if (Mathf::FAbs(level) > maxLevel)
        {
            maxLevel = Mathf::FAbs(level);
        }

        float temp = Mathf::ATan2(position[1], position[2])/Mathf::PI;
        float width = 0.5f*(1.0f + temp);  // in [0,1)
        if (width < 0.0f)
        {
            width = 0.0f;
        }
        else if (width >= 1.0f)
        {
            width = 0.999999f;
        }

        float height = 0.5f*position[2]; // in [0,1)
        if (height < 0.0f)
        {
            height = 0.0f;
        }
        else if (height >= 1.0f)
        {
            height = 0.999999f;
        }

        vba.TCoord<Float2>(0, i) = Float2(width, height);
    }

    int numTriangles = (int)triangles.size();
    int numIndices = 3*numTriangles;
    IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, sizeof(int));
    int* indices = (int*)ibuffer->GetData();
    for (i = 0; i < numTriangles; ++i)
    {
        *indices++ = triangles[i].V[0];
        *indices++ = triangles[i].V[1];
        *indices++ = triangles[i].V[2];
    }

    mMesh = new TriMesh(vformat, vbuffer, ibuffer);

    // Create a texture effect for the ball.
    std::string path = Environment::GetPathR("BallTexture.wmtf");
    Texture2D* texture = Texture2D::LoadWMTF(path);
    mMesh->SetEffectInstance(Texture2DEffect::CreateUniqueInstance(texture,
        Shader::SF_LINEAR, Shader::SC_REPEAT, Shader::SC_REPEAT));

    mMaxIterations = 8;
    mErrorTolerance = maxLevel;
    CreateSmoother();
    Update(0.0f);
}
//----------------------------------------------------------------------------
void DeformableBall::CreateSmoother ()
{
    int numVertices = mMesh->GetVertexBuffer()->GetNumElements();
    mNormal = new1<AVector>(numVertices);
    mMean = new1<AVector>(numVertices);
    mNeighborCount = new1<int>(numVertices);

    // Count the number of vertex neighbors.
    memset(mNeighborCount, 0, numVertices*sizeof(int));
    int numTriangles = mMesh->GetNumTriangles();
    const int* indices = (const int*)mMesh->GetIndexBuffer()->GetData();
    for (int i = 0; i < numTriangles; ++i)
    {
        mNeighborCount[*indices++] += 2;
        mNeighborCount[*indices++] += 2;
        mNeighborCount[*indices++] += 2;
    }
}
//----------------------------------------------------------------------------
void DeformableBall::Update (float time)
{
    int numVertices = mMesh->GetVertexBuffer()->GetNumElements();
    int numTriangles = mMesh->GetNumTriangles();
    const int* indices = (const int*)mMesh->GetIndexBuffer()->GetData();
    VertexBufferAccessor vba(mMesh);

    memset(mNormal, 0, numVertices*sizeof(AVector));
    memset(mMean, 0, numVertices*sizeof(AVector));

    int i;
    for (i = 0; i < numTriangles; ++i)
    {
        int i0 = *indices++;
        int i1 = *indices++;
        int i2 = *indices++;

        AVector v0 = vba.Position<Float3>(i0);
        AVector v1 = vba.Position<Float3>(i1);
        AVector v2 = vba.Position<Float3>(i2);

        AVector edge1 = v1 - v0;
        AVector edge2 = v2 - v0;
        AVector normal = edge1.Cross(edge2);

        mNormal[i0] += normal;
        mNormal[i1] += normal;
        mNormal[i2] += normal;

        mMean[i0] += v1 + v2;
        mMean[i1] += v2 + v0;
        mMean[i2] += v0 + v1;
    }

    for (i = 0; i < numVertices; ++i)
    {
        mNormal[i].Normalize();
        mMean[i] /= (float)mNeighborCount[i];
    }

    for (i = 0; i < numVertices; ++i)
    {
        AVector position = vba.Position<Float3>(i);
        if (VertexInfluenced(i, time, position))
        {
            AVector localDiff = mMean[i] - position;
            AVector surfaceNormal = localDiff.Dot(mNormal[i])*mNormal[i];
            AVector tangent = localDiff - surfaceNormal;
            float tWeight = GetTangentWeight(i, time, position);
            float nWeight = GetNormalWeight(i, time, position);
            position += tWeight*tangent + nWeight*mNormal[i];
            vba.Position<Float3>(i) = position;
        }
    }
}
//----------------------------------------------------------------------------
bool DeformableBall::DoSimulationStep (float realTime)
{
    float time = fmodf(realTime, mPeriod);

    if (mMinActive < time && time < mMaxActive)
    {
        // Deform the mesh.
        mDeforming = true;
        Update(time);

        if (mDoAffine)
        {
            // Nonuniform scaling as a hack to make it appear that the body is
            // compressing in the z-direction.  The transformations only
            // affect the display.  If the actual body coordinates were needed
            // for other physics, you would have to modify the mesh vertices.
            //
            // The x- and y-scales vary from 1 to 1.1 to 1 during the time
            // interval [(p-d)/2,(p+d)/2].  The z-scale is the inverse of this
            // scale.  (Expand radially, compress in z-direction.)
            const float maxExpand = 0.1f;
            float amp = 4.0f*maxExpand*mInvActiveRange;
            float xyScale = 1.0f + amp*(time-mMinActive)*(mMaxActive-time);
            float zScale = 1.0f/xyScale;
            mMesh->LocalTransform.SetScale(APoint(xyScale, xyScale, zScale));
        }

        // Deformation requires update of bounding sphere.
        mMesh->UpdateModelSpace(Visual::GU_MODEL_BOUND_ONLY);

        // update occurred, application should update the scene graph
        return true;
    }

    if (mDeforming)
    {
        // Force restoration of body to its initial state on a transition
        // from deforming to nondeforming.
        mDeforming = false;
        Update(0.0f);
        if (mDoAffine)
        {
            mMesh->LocalTransform.SetRotate(HMatrix::IDENTITY);
        }
        mMesh->UpdateModelSpace(Visual::GU_MODEL_BOUND_ONLY);
        return true;
    }

    mDeforming = false;
    return false;
}
//----------------------------------------------------------------------------
bool DeformableBall::VertexInfluenced (int, float time,
    const AVector& position)
{
    float rSqr = position.SquaredLength();
    return rSqr < 1.0f && mMinActive < time && time < mMaxActive;
}
//----------------------------------------------------------------------------
float DeformableBall::GetTangentWeight (int, float, const AVector&)
{
    return 0.5f;
}
//----------------------------------------------------------------------------
float DeformableBall::GetNormalWeight (int i, float time,
    const AVector& position)
{
    // Find root of F along line origin+s*dir using Newton's method.
    float s = 0.0f;
    for (int iter = 0; iter < mMaxIterations; ++iter)
    {
        // Point of evaluation.
        AVector evalPosition = position + s*mNormal[i];

        // Get F(pos,time) and Grad(F)(pos,time).
        float function;
        AVector gradient;
        ComputeFunction(evalPosition, time, function, gradient);
        if (Mathf::FAbs(function) < mErrorTolerance)
        {
            return s;
        }

        // Get directional derivative Dot(dir,Grad(F)(pos,time)).
        float derFunction = mNormal[i].Dot(gradient);
        if (Mathf::FAbs(derFunction) < mErrorTolerance)
        {
            // Derivative too close to zero, no change.
            return 0.0f;
        }

        s -= function/derFunction;
    }

    // Method failed to converge within iteration budget, no change.
    return 0.0f;
}
//----------------------------------------------------------------------------
void DeformableBall::ComputeFunction (const AVector& position, float time,
    float& function, AVector& gradient)
{
    // Level function is L(X,t) = F(X) + D(X,t) where F(X) = 0 defines the
    // initial body.

    // Compute F(X) = x^2 + y^2 + z^2 - 2*z and Grad(F)(X).
    float rSqr = position.SquaredLength();
    float F = rSqr - 2.0f*position[2];
    AVector FGrad = 2.0f*(position - AVector::UNIT_Z);

    // Compute D(X,t) = A(t)*G(X).  The duration is d and the period is p.
    // The amplitude is
    //   A(t) = 0, t in [0,p/2-d]
    //          [t-(p/2-d)][(p/2+d)-t]/d^2, t in [p/2-d,p/2+d]
    //          0, t in [p/2+d,p]
    // The spatial component is G(X) = 1 - (x^2 + y^2 + z^2)
    float D;
    AVector DGrad;
    if (rSqr < 1.0f && mMinActive < time && time < mMaxActive)
    {
        float amp = GetAmplitude(time);
        D = amp*(1.0f - rSqr);
        DGrad = -2.0f*amp*position;
    }
    else
    {
        D = 0.0f;
        DGrad = AVector::ZERO;
    }

    function = F + D;
    gradient = FGrad + DGrad;
}
//----------------------------------------------------------------------------
