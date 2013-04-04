// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "RandomController.h"
#include "Wm5Polypoint.h"
#include "Wm5Renderer.h"
#include "Wm5VertexBufferAccessor.h"
using namespace Wm5;

WM5_IMPLEMENT_RTTI(Wm5, PointController, RandomController);
WM5_IMPLEMENT_STREAM(RandomController);
WM5_IMPLEMENT_FACTORY(RandomController);
WM5_IMPLEMENT_DEFAULT_STREAM(PointController, RandomController);
WM5_IMPLEMENT_DEFAULT_NAMES(PointController, RandomController);

//----------------------------------------------------------------------------
RandomController::RandomController ()
{
}
//----------------------------------------------------------------------------
void RandomController::UpdatePointMotion (float)
{
    Polypoint* points = StaticCast<Polypoint>(mObject);

    VertexBufferAccessor vba(points);
    const int numPoints = vba.GetNumVertices();
    for (int i = 0; i < numPoints; ++i)
    {
        Float3& position = vba.Position<Float3>(i);
        for (int j = 0; j < 3; ++j)
        {
            position[j] += 0.01f*Mathf::SymmetricRandom();
            if (position[j] > 1.0f)
            {
                position[j] = 1.0f;
            }
            else if (position[j] < -1.0f)
            {
                position[j] = -1.0f;
            }
        }
    }

    Renderer::UpdateAll(points->GetVertexBuffer());
}
//----------------------------------------------------------------------------
