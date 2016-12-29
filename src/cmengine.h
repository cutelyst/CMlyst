#ifndef CMENGINE_H
#define CMENGINE_H

#include "libCMS/engine.h"

class CMEngine
{
public:
    CMEngine();
    virtual ~CMEngine();

    CMS::Engine *engine = 0;
};

#endif // CMENGINE_H
