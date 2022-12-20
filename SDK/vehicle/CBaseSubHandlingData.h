#pragma once
#include "CHandlingObject.h"
#include "../enums/eHandlingType.h"

class CBaseSubHandlingData : public CHandlingObject
{
public:
    virtual eHandlingType GetHandlingType() = 0;
    virtual void OnPostLoad() = 0;
    
};