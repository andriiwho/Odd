#include "RootObjRegistry.h"
#include "RootObj.h"

namespace Odd::Internal
{
    RootObjRegistry* GRootObjRegistry = nullptr;

    RootObjectID RootObjRegistry::RegisterRootObj(RootObj* pObj)
    {
        std::unique_lock lck(m_NewObjMutex);
        if (!m_FreeIDs.empty())
        {
            RootObjectID id = m_FreeIDs.back();
            m_FreeIDs.pop_back();
            m_ActiveRootObjects[scast(size_t, id)] = pObj;
            return id;
        }

        RootObjectID id = scast(RootObjectID, m_ActiveRootObjects.size());
        m_ActiveRootObjects.push_back(pObj);
        return id;
    }

    void RootObjRegistry::MarkRootObjectExpired(RootObjectID pObj)
    {
        std::unique_lock lck(m_ExpireMutex);
        m_ExpiredRootObjects.push_back(pObj);
    }

    void RootObjRegistry::FlushExpiredRootObjects()
    {
        if (m_ExpiredRootObjects.empty())
            return;

        std::unique_lock lck(m_ExpireMutex);
        for (RootObjectID id : m_ExpiredRootObjects)
        {
            size_t index = scast(size_t, id);
            if (index < m_ActiveRootObjects.size())
            {
                RootObj* pObj = m_ActiveRootObjects[index];
                if (pObj)
                {
                    OddDelete(pObj);
                    m_ActiveRootObjects[index] = nullptr;

                    std::unique_lock lck2(m_NewObjMutex);
                    m_FreeIDs.push_back(id);
                }
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

} // namespace Odd::Internal