#pragma once

#include "CPed.h"

class CPedFactory
{
public:
    char pad_0000[8]; //0x0000
    CPed* m_local_ped; //0x0008
}; //Size: 0x0010
static_assert(sizeof(CPedFactory) == 0x10);
