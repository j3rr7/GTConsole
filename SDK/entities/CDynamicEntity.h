#pragma once

#include "CEntity.h"
#include "../network/netObject.h"

#include <cstdint>

namespace rage
{
    class CDynamicEntity : public CEntity
    {
    public:
        class rage::netObject *m_net_object; //0x00D0
        char gapD8[16];
        uint64_t qwordE8;
    };
    static_assert(sizeof(CDynamicEntity) == 0xF0);
}
