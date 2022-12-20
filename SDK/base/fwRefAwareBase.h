#pragma once

#include "datBase.h"
#include "fwRefAwareBaseImpl.h"

namespace rage
{
    class fwRefAwareBase : public fwRefAwareBaseImpl<datBase>
    {
    };
    static_assert(sizeof(fwRefAwareBase) == 0x10);
}