#include "RootObjRegistry.h"
#include "RootObj.h"

namespace Odd::Internal
{
    RootObjRegistry* GRootObjRegistry = nullptr;

    union RootObjIDUnion
    {
        RootObjectID Handle;
        struct
        {
            uint32_t Id;
            uint32_t Version;
        };
    };

    static uint32_t GetObjID(RootObjectID id)
    {
        RootObjIDUnion u{
            .Handle = id,
        };
        return u.Id;
    }

    static uint32_t GetObjVersion(RootObjectID id)
    {
        RootObjIDUnion u{
            .Handle = id,
        };
        return u.Version;
    }

    RootObjectID RootObjRegistry::RegisterRootObj(RootObj* pObj)
    {
        std::unique_lock lck(m_NewObjMutex);
        if (!m_FreeIDs.empty())
        {
            RootObjectID id = m_FreeIDs.back();
            m_FreeIDs.pop_back();

            // Convert ID to index
            RootObjIDUnion u{
                .Handle = id,
            };

            // Here we get the slot, which already contains the correct version.
            // But the free handle doesn't. So we need to adjust the handle.
            auto& slot = m_ActiveRootObjects[scast(size_t, GetObjID(id))];
            slot.Obj = pObj;

            // Set the new generation version and convert to handle.
            u.Version = slot.Generation;
            return u.Handle;
        }

        RootObjIDUnion u{
            .Handle = scast(RootObjectID, m_ActiveRootObjects.size()),
        };
        m_ActiveRootObjects.push_back({
            .Obj = pObj,
            .Generation = 0,
        });
        return u.Handle;
    }

    void RootObjRegistry::MarkRootObjectExpired(RootObjectID pObj)
    {
        std::unique_lock lck(m_ExpireMutex);
        auto&            slot = GetSlot(pObj);

        m_ExpiredRootObjects.push_back(slot.Obj);
        m_FreeIDs.push_back(pObj);

        // Increase the pointer generation.
        slot.Generation++;
    }

    void RootObjRegistry::FlushExpiredRootObjects()
    {
        if (m_ExpiredRootObjects.empty())
            return;

        std::unique_lock lck(m_ExpireMutex);
        for (RootObj* pObj : m_ExpiredRootObjects)
        {
            if (pObj)
            {
                OddDelete(pObj);
            }
        }
        m_ExpiredRootObjects.clear();
    }

    void RootObjRegistry::Init()
    {
        oddValidateMsg(GRootObjRegistry == nullptr, "RootObjRegistry already initialized.");
        GRootObjRegistry = OddNew<RootObjRegistry>();
        oddValidateMsg(GRootObjRegistry != nullptr, "Failed to create RootObjRegistry.");
    }

    void RootObjRegistry::Shutdown()
    {
        if (GRootObjRegistry)
        {
            GRootObjRegistry->FlushExpiredRootObjects();
            OddDelete(GRootObjRegistry);
            GRootObjRegistry = nullptr;
        }
    }

    // We would probably want to preallocate some space here in the future
    RootObjRegistry::RootObjRegistry()
    {
        m_ActiveRootObjects.reserve(1024);
        m_ExpiredRootObjects.reserve(1024);
        m_FreeIDs.reserve(1024);
    }

    Odd::Internal::RootObjectSlot& RootObjRegistry::GetSlot(RootObjectID id)
    {
        RootObjIDUnion u{
            .Handle = id,
        };

        oddValidate(m_ActiveRootObjects.size() > u.Id);
        return m_ActiveRootObjects.at(u.Id);
    }

    const RootObjectSlot& RootObjRegistry::GetSlot(RootObjectID id) const
    {
        return cncast(RootObjRegistry*, this)->GetSlot(id);
    }

    RootObj* RootObjRegistry::TryGet(RootObjectID id) const
    {
        if (m_ActiveRootObjects.size() <= GetObjID(id))
            return nullptr;

        std::unique_lock lck(m_ExpireMutex);
        const auto& slot = GetSlot(id);
        if (slot.Generation != GetObjVersion(id))
            return nullptr;

        return slot.Obj;
    }

} // namespace Odd::Internal