// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.9.0 (2012/08/05)

#ifndef TEMPLATEXC44_H
#define TEMPLATEXC44_H

class TemplateXC44
{
public:
    TemplateXC44 (const char* projectName);
    ~TemplateXC44 ();

private:
    enum { NUM_LINES = 574 };
    static char* msLines[NUM_LINES];
};

#endif
