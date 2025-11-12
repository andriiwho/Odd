#pragma once

#include "OddCore.h"

#include <mutex>

namespace Odd
{
    class RootObj;
    namespace Internal
    {
        // Firts 32 bits - ID in obj array
        // Second 32 bits - generation of object to avoid resurrecting expired objects.
        enum class RootObjectID : uint64_t
        {
            Invalid = UINT64_MAX
        };

        struct RootObjectSlot
        {
            RootObj* Obj;
            uint32_t Generation;
        };

        class RootObjRegistry final
        {
        public:
            RootObjectID RegisterRootObj(RootObj* pObj);
            void         MarkRootObjectExpired(RootObjectID pObj);

            void     FlushExpiredRootObjects();
            RootObj* TryGet(RootObjectID id) const;

            static void Init();
            static void Shutdown();

            RootObjRegistry();
            ~RootObjRegistry() = default;

        private:
            RootObjRegistry(const RootObjRegistry&) = delete;
            RootObjRegistry& operator=(const RootObjRegistry&) = delete;

            RootObjectSlot&       GetSlot(RootObjectID id);
            const RootObjectSlot& GetSlot(RootObjectID id) const;

        private:
            Vector<RootObjectSlot> m_ActiveRootObjects{};
            Vector<RootObj*>       m_ExpiredRootObjects{};
            Vector<RootObjectID>   m_FreeIDs{};

            mutable std::mutex m_NewObjMutex;
            mutable std::mutex m_ExpireMutex;
        };

        extern RootObjRegistry* GRootObjRegistry;
    } // namespace Internal
} // namespace Odd