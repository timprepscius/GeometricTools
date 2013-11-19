// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "BeadSlide.h"
#include "PhysicsModule.h"

WM5_CONSOLE_APPLICATION(BeadSlide);

//----------------------------------------------------------------------------
BeadSlide::BeadSlide ()
    :
    ConsoleApplication("SamplePhysics/BeadSlide")
{
}
//----------------------------------------------------------------------------
void BeadSlide::Simulation ()
{
    // Set up the physics module.
    PhysicsModule module;
    module.Gravity = 1.0;
    module.Mass = 0.1;

    double time = 0.0;
    double deltaTime = 0.001;
    double q = 1.0;
    double qDot = 0.0;
    module.Initialize(time, deltaTime, q, qDot);

    // Run the simulation.
    std::string path = ThePath + "simulation.txt";
    std::ofstream outFile(path.c_str());
    outFile << "time  q  qder  position" << std::endl;
    const int imax = 2500;
    for (int i = 0; i < imax; ++i)
    {
        double x = q, y = q*q, z = q*y;

        char message[512];
        sprintf(message, "%5.3lf %+16.8lf %+16.8lf %+8.4lf %+8.4lf %+8.4lf",
            time, q, qDot, x, y, z);
        outFile << message << std::endl;

        module.Update();

        time = module.GetTime();
        q = module.GetQ();
        qDot = module.GetQDot();
    }
    outFile.close();
}
//----------------------------------------------------------------------------
int BeadSlide::Main (int, char**)
{
    Simulation();
    return 0;
}
//----------------------------------------------------------------------------
