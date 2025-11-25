#pragma once

#include "OddMalloc.h"

namespace Odd
{
    class RHICommandContext;

    class RHICommand
    {
    public:
        virtual void ExecuteAndClear(RHICommandContext& cmdCtx) = 0;

    private:
        friend class RHICommandList;

        class RHICommand* m_Next{};
    };

    class RHICommandList
    {
    public:
        void Execute(RHICommandContext& cmdCtx);

        void BeginRecording(size_t commandPoolCapacity = 0);
        void EndRecording();

    private:
        template <typename T>
        T* AllocCmd()
        {
            // Allocate the command
            T* cmd = scast(T*, m_CommandAllocator.Allocate(sizeof(T)));

            // If we don't have a first command assign it.
            if (m_First == nullptr)
            {
                m_First = cmd;
            }

            // If we have the last command, set the m_Next of the last command to the new command.
            else if (m_Last)
            {
                m_Last->m_Next = cmd;
            }

            // Set created command as the last one.
            m_Last = cmd;
            return cmd;
        }

        LinearAllocator m_CommandAllocator{};
        RHICommand*     m_First{};
        RHICommand*     m_Last{};
    };
} // namespace Odd