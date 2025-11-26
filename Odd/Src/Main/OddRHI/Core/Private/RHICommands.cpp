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
        if (commandPoolCapacity > 0 && commandPoolCapacity > m_CommandAllocator.GetCapacity())
        {
            m_CommandAllocator.ResetAndResize(commandPoolCapacity);
        }
        else
        {
            m_CommandAllocator.Reset();
            m_First = nullptr;
            m_Last = nullptr;
        }
    }

    void RHICommandList::EndRecording()
    {
    }

    void RHICommandList::TransitionResource(RHIResource* resource, const RHIResourceTransitionInfo& info)
    {
        RHICommand_TransitionResource* cmd = AllocCmd<RHICommand_TransitionResource>();
        cmd->Resource = resource;
        cmd->TransitionInfo = info;
    }

}