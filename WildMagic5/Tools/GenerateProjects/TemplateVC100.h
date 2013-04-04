// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.2.1 (2012/07/04)

#ifndef TEMPLATEVC100_H
#define TEMPLATEVC100_H

class TemplateVC100
{
public:
    static void GenerateDx9 (const char* projectName);
    static void GenerateWgl (const char* projectName);

private:
    static void Generate (const char* projectName, const char* prefix,
        char* projectLines[]);

    enum
    {
        NUM_PROJECT_LINES = 330,
        NUM_FILTER_LINES = 23
    };

    static char* msDx9ProjectLines[NUM_PROJECT_LINES];
    static char* msWglProjectLines[NUM_PROJECT_LINES];
    static char* msFilterLines[NUM_FILTER_LINES];
};

#endif
