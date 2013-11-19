// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.1.0 (2010/04/14)

#ifndef BIPEDMANAGER_H
#define BIPEDMANAGER_H

#include "Wm5Graphics.h"
using namespace Wm5;

class BipedManager
{
public:
    // Construction and destruction.
    BipedManager (const std::string& rootPath, const std::string& name);
    ~BipedManager ();

    // Member access.
    inline Node* GetRoot ();

    // Get the extreme times for all the controllers of the animation.
    inline void GetIdle (double& minTime, double& maxTime) const;
    inline void GetWalk (double& minTime, double& maxTime) const;
    inline void GetRun (double& minTime, double& maxTime) const;

    // Set time sampler parameters.
    inline void SetIdle (double frequency, double phase);
    inline void SetWalk (double frequency, double phase);
    inline void SetRun (double frequency, double phase);

    // Select animations.
    inline void DoIdle ();
    inline void DoWalk ();
    inline void DoRun ();
    inline void DoIdleWalk ();
    inline void DoWalkRun ();

    // Set blending weight.
    inline void SetIdleWalk (float weight);
    inline void SetWalkRun (float weight);

    // Finite state machine for switching and blending among animations.
    
    // The input idleWalkCount is the maximum number of times Update samples
    // the blend of idle and walk before transitioning to idle or walk.  The
    // input walkCount is the maximum number of times Update samples the
    // walk when blendWalkToRun is 'true' in the Update function.  The input
    // walkRunCount is the maximum number of times Update samples the blend
    // of walk and run before transitioning to walk or run.
    void Initialize (int idleWalkCount, int walkCount, int walkRunCount);

    // Select and sample the appropriate animation.
    //
    // If blendIdleToWalk is 'false', the input blendWalkToRun is ignored.
    // The machine transitions from its current animation state (determined
    // by count) to the idle animation (count is decremented to zero).
    //
    // If blendIdleToWalk is 'true' and blendWalkToRun is 'false', then the
    // machine transitions from its current animation state (determined by
    // count) to the walk-animation state (count is incremented).  Once the
    // machine enters the walk-animation state, it stays in that state.
    //
    // If blendIdleToWalk is 'true' and blendWalkToRun is 'true', then the
    // machine transitions from its current animation state (determined by
    // count) to the run-animation state (count is incremented).  Once the
    // machine enters the run-animation state, it stays in that state.
    void Update (bool blendIdleToWalk, bool blendWalkToRun);

    // Get the speed of the biped.  This depends on the current animation
    // state.  The speed here is dimensionless, measured as the ratio
    // mCount/mCountMax[ANIM_RUN], which is in [0,1].
    float GetSpeed () const;

private:
    // Loading support.
    typedef std::vector<std::string> StringArray;
    typedef std::map<std::string, Spatial*> SpatialMap;
    typedef std::pair<Node*, TransformControllerPtr> NodeCtrl;
    typedef std::vector<NodeCtrl> NodeCtrlArray;

    class PreSpatial
    {
    public:
        Spatial* Associate;
        StringArray ChildNames;
    };
    typedef std::vector<PreSpatial*> PreSpatialArray;

    class PreSkin
    {
    public:
        SkinController* Associate;
        StringArray BoneNames;
    };
    typedef std::vector<PreSkin*> PreSkinArray;

    PreSpatial* LoadNode (const std::string& rootPath,
        const std::string& name);

    PreSpatial* LoadMesh (const std::string& rootPath,
        const std::string& name, VertexFormat* vformat,
        VisualEffectInstance* instance);

    PreSkin* LoadSkinController (const std::string& rootPath,
        const std::string& name);

    TransformController* LoadTransformController (const std::string& rootPath,
        const std::string& name, const std::string& animation);

    // Run-time support.
    void GetAnimation (const NodeCtrlArray& ncArray, double& minTime,
        double& maxTime) const;
    void SetAnimation (NodeCtrlArray& ncArray, double frequency,
        double phase);
    void SetBlendAnimation (NodeCtrlArray& ncArray, float weight);
    void DoAnimation (NodeCtrlArray& ncArray);

    // The biped and animation sequences.
    NodePtr mRoot;
    NodeCtrlArray mIdleArray, mWalkArray, mRunArray;
    NodeCtrlArray mIdleWalkArray, mWalkRunArray;

    // Finite state machine.
    enum Animation
    {
        ANIM_IDLE,
        ANIM_IDLE_WALK,
        ANIM_WALK,
        ANIM_WALK_RUN,
        ANIM_RUN,
        NUM_STATES
    };

    void ContinueIdleWalk ();
    void ContinueWalkRun ();
    void ContinueRunWalk ();
    void ContinueWalkIdle ();
    void TransitionIdleToIdleWalk ();
    void TransitionIdleWalkToWalk ();
    void TransitionWalkToWalkRun ();
    void TransitionWalkRunToRun ();
    void TransitionRunToRunWalk ();
    void TransitionRunWalkToWalk ();
    void TransitionWalkToWalkIdle ();
    void TransitionWalkIdleToIdle ();

    Animation mState;
    int mCount, mCountMax[NUM_STATES];
    float mWeight, mDeltaWeight0, mDeltaWeight1;
};

#include "BipedManager.inl"

#endif
