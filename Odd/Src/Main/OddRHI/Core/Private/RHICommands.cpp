#include "RHICommands.h"

namespace Odd
{

    void RHICommandList::Execute(RHICommandContext& cmdCtx)
    {
        RHICommand* cmd = m_First;
        if (!cmd)
        {
            return;
        }

        do
        {
            cmd->ExecuteAndClear(cmdCtx);
        }
        while (cmd = cmd->m_Next);
    }

    void RHICommandList::BeginRecording(size_t commandPoolCapacity /*= 0*/)
    {
        if (commandPoolCapacity > 0)
        {
            m_CommandAllocator.ResetAndResize(commandPoolCapacity);
        }
        else
        {
            m_CommandAllocator.Reset();
        }
    }

    void RHICommandList::EndRecording()
    {
    }

}