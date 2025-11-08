#pragma once

#include "OddCore.h"

#include <mutex>

namespace Odd
{
    class RootObj;
    namespace Internal
    {
        enum class RootObjectID : uint64_t
        {
        };

        class RootObjRegistry final
        {
        public:
            RootObjectID RegisterRootObj(RootObj* pObj);
            void         MarkRootObjectExpired(RootObjectID pObj);

            void FlushExpiredRootObjects();

            static void Init();
            static void Shutdown();

            RootObjRegistry();
            ~RootObjRegistry() = default;

        private:
            RootObjRegistry(const RootObjRegistry&) = delete;
            RootObjRegistry& operator=(const RootObjRegistry&) = delete;

        private:
            Vector<RootObj*>     m_ActiveRootObjects{};
            Vector<RootObjectID> m_ExpiredRootObjects{};
            Vector<RootObjectID> m_FreeIDs{};

            std::mutex m_NewObjMutex;
            std::mutex m_ExpireMutex;
        };

        extern RootObjRegistry* GRootObjRegistry;
    } // namespace Internal
} // namespace Odd