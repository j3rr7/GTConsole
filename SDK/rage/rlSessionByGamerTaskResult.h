#pragma once
#include "rlGamerHandle.h"
#include "rlSessionInfo.h"

namespace rage
{
    class rlSessionByGamerTaskResult
    {
    public:
        rlGamerHandle m_gamer_handle{ 0 };
        rlSessionInfo m_session_info;
    };
}
