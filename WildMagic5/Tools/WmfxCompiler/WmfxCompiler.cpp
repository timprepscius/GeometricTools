// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/04/14)

#include "WmfxCompiler.h"
#include "FxCompiler.h"

WM5_CONSOLE_APPLICATION(WmfxCompiler);

//----------------------------------------------------------------------------
WmfxCompiler::WmfxCompiler ()
    :
    ConsoleApplication("Tools/WmfxCompiler")
{
}
//----------------------------------------------------------------------------
WmfxCompiler::~WmfxCompiler ()
{
}
//----------------------------------------------------------------------------
int WmfxCompiler::Main (int numArguments, char** arguments)
{
    if (numArguments != 2 && numArguments != 3)
    {
        std::cout << "Usage: WmfxCompiler filename  "
            << "(compile and create wmfx files)" << std::endl;
        std::cout << "Usage: WmfxCompiler -a filename  "
            << "(create wmfx from already compiled files)" << std::endl;
        return -1;
    }

    std::string fxName;
    bool alreadyCompiled;
    if (numArguments == 2)
    {
        fxName = std::string(arguments[1]);
        alreadyCompiled = false;
    }
    else
    {
        std::string option(arguments[1]);
        if (option != "-a")
        {
            std::cout << "Usage: WmfxCompiler filename  "
                << "(compile and create wmfx files)" << std::endl;
            std::cout << "Usage: WmfxCompiler -a filename  "
                << "(create wmfx from already compiled files)" << std::endl;
            return -2;
        }
        fxName = std::string(arguments[2]);
        alreadyCompiled = true;
    }

    std::string::size_type begin = fxName.find(".fx", 0);
    if (begin != std::string::npos)
    {
        // Strip off the ".fx" extension.
        fxName = fxName.substr(0, begin);
    }

    FxCompiler compiler(fxName, alreadyCompiled);

    // Display messages to console window and to log file.
    std::cout << std::endl;
    if (compiler.Messages.size() > 0)
    {
        std::string logname = fxName + ".logfile.txt";
        FILE* logfile = fopen(logname.c_str(), "wt");
        std::vector<std::string>::iterator iter = compiler.Messages.begin();
        std::vector<std::string>::iterator end = compiler.Messages.end();
        for (/**/; iter != end; ++iter)
        {
            std::cout << *iter;
            if (logfile)
            {
                fprintf(logfile, "%s", iter->c_str());
            }
        }
        if (logfile)
        {
            fclose(logfile);
        }
    }
    else
    {
        std::cout << "Compiled " << fxName << " as-is.";
    }
    std::cout << std::endl;

    return 0;
}
//----------------------------------------------------------------------------
