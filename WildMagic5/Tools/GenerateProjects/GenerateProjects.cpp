// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.3 (2012/08/05)

#include <iostream>
#include "TemplateVC100.h"
#include "TemplateXC44.h"

//----------------------------------------------------------------------------
int main (int numArguments, char** arguments)
{
    if (numArguments != 2)
    {
        std::cout << "usage: GenerateProjects projectname" << std::endl;
        return -1;
    }

    const char* projectName = arguments[1];
    TemplateVC100::GenerateDx9(projectName);
    TemplateVC100::GenerateWgl(projectName);
    TemplateXC44 generate(projectName);
    return 0;
}
//----------------------------------------------------------------------------
