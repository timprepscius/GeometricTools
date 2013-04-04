// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef CREATEENVELOPE_H
#define CREATEENVELOPE_H

#include "Rational.h"
#include "Wm5Vector2.h"
using namespace Wm5;

class CreateEnvelope
{
public:
    CreateEnvelope (int numVertices, const Vector2f* vertices,
        int numIndices, const int* indices, int& numEnvelopeVertices,
        Vector2f*& envelopeVertices);

private:
    typedef std::map<RScalar,RPoint2> EdgeMap;

    void InsertParameter (EdgeMap& edgeMap, const RScalar& t);
    void UpdateEdges (EdgeMap& edgeMap0, EdgeMap& edgeMap1);
    void UpdateAllEdges (int numEdges, EdgeMap** edgeMaps);

    static const RScalar ZERO;
    static const RScalar ONE;
};

#endif
