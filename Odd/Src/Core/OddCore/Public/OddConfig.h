#pragma once

#include "Macros.h"
#include "Types.h"
#include "yaml-cpp/node/node.h"
#include "yaml-cpp/node/convert.h"

#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace Odd
{
    // Config should be the first thing that's loaded before any other system.
    // Memory pool is unavailable at this moment, so we need to load the config
    // using standard allocator.
    // ATTENTION: Config currently uses a lot of default system heap and doesn't utilize Odd allocators due to the nature of yaml-cpp.
    // This will be fixed, when yaml-cpp gets adapted to Odd's allocators.
    // Use with caution.
    class ConfigRegistry
    {
    public:
        ODD_NO_MOVE_COPY_CONSTRUCTORS(ConfigRegistry);

        ConfigRegistry();
        ~ConfigRegistry();

        template <typename T>
        std::optional<T> Get(std::string_view path) const
        {
            if (auto value = GetWithDottedPath(path))
            {
                // TODO: A little hack for now.
                if constexpr (std::is_same_v<T, String>)
                    return ToString(value->as<std::string>());
                else if constexpr (std::is_same_v<T, uint32_t>)
                    return scast(uint32_t, value->as<int>());
                else
                    return value->as<T>();
            }

            return std::nullopt;
        }

        template <typename T>
        T Get(std::string_view path, T&& defaultValue) const
        {
            return Get<T>(path).value_or(std::forward<T>(defaultValue));
        }

    private:
        YAML::Node                LoadConfigFile(const std::string& file);
        std::optional<YAML::Node> GetWithDottedPath(std::string_view path) const;

        YAML::Node m_Config;
    };

    extern ConfigRegistry* GConfig;

    namespace Internal
    {
        void InitializeConfig();
        void ShutdownConfig();
    } // namespace Internal
} // namespace Odd

#define ODD_CONF(type, name) Odd::GConfig->Get<type>(name);
#define ODD_CONF_OR(type, name, def) Odd::GConfig->Get<type>(name, def)