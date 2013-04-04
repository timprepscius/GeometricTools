// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "BloodCellController.h"
#include "Wm5Renderer.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, ParticleController, BloodCellController);
WM5_IMPLEMENT_STREAM(BloodCellController);
WM5_IMPLEMENT_FACTORY(BloodCellController);
WM5_IMPLEMENT_DEFAULT_STREAM(ParticleController, BloodCellController);
WM5_IMPLEMENT_DEFAULT_NAMES(ParticleController, BloodCellController);

//----------------------------------------------------------------------------
BloodCellController::BloodCellController ()
{
}
//----------------------------------------------------------------------------
void BloodCellController::UpdatePointMotion (float)
{
    Particles* particles = StaticCast<Particles>(mObject);

    const int numParticles = particles->GetNumParticles();
    Float4* posSizes = particles->GetPositionSizes();
    for (int i = 0; i < numParticles; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            posSizes[i][j] += 0.01f*Mathf::SymmetricRandom();
            if (posSizes[i][j] > 1.0f)
            {
                posSizes[i][j] = 1.0f;
            }
            else if (posSizes[i][j] < -1.0f)
            {
                posSizes[i][j] = -1.0f;
            }
        }

        posSizes[i][3] *= (1.0f + 0.01f*Mathf::SymmetricRandom());
        if (posSizes[i][3] > 0.25f)
        {
            posSizes[i][3] = 0.25f;
        }
    }

    Renderer::UpdateAll(particles->GetVertexBuffer());
}
//----------------------------------------------------------------------------
