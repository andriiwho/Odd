#include "OddConfig.h"

#include <filesystem>

// TODO: Fork yaml-cpp and add support for custom allocators.
#include <yaml-cpp/yaml.h>

namespace Odd
{
    namespace fs = std::filesystem;

    ConfigRegistry* GConfig;

    static std::string LoadProjectRoot(const YAML::Node& node, const std::string& targetName)
    {
        auto conf = node[targetName];
        if (!conf)
            return {};

        auto path = conf["RootPath"];
        if (!path)
            return {};

        return path.as<std::string>();
    }

    static std::string_view TrimStringView(std::string_view view)
    {
        while (view.starts_with(" "))
            view = view.substr(1);

        while (view.ends_with(" "))
            view = view.substr(0, view.length() - 1);

        return view;
    }

    static std::vector<std::string> SplitDottedPath(std::string_view path)
    {
        std::vector<std::string> tokens;
        std::string_view         copied = path;

        size_t      pos = 0;
        std::string token;
        while ((pos = copied.find('.')) != std::string_view::npos)
        {
            token = copied.substr(0, pos);
            tokens.push_back(token);
            copied = copied.substr(pos + 1);
        }
        tokens.push_back(std::string(copied));

        return tokens;
    }

    static std::string MakeConfigPathForRoot(std::string root)
    {
        if (root.empty())
            return {};

        const auto rootPath = fs::current_path() / root;
        const auto configPath = fs::current_path() / root / "Config";
        const auto projDirName = rootPath.filename();

        const auto outPath = configPath / projDirName;
        return outPath.string() + ".yaml";
    }

    ConfigRegistry::ConfigRegistry()
    {
        // Load main config file.
        // There should be main config file in the working directory.
        const auto oddProjectPath = fs::current_path() / "OddProject.yaml";

        oddValidate(fs::exists(oddProjectPath));
        YAML::Node projectConfig = LoadConfigFile(oddProjectPath.string());

        auto engineRoot = LoadProjectRoot(projectConfig, "Engine");
        oddValidate(!engineRoot.empty());
        auto engineConf = LoadConfigFile(MakeConfigPathForRoot(engineRoot));

        auto runtimeRoot = LoadProjectRoot(projectConfig, "Runtime");
        oddValidate(!runtimeRoot.empty());
        auto runtimeConf = LoadConfigFile(MakeConfigPathForRoot(runtimeRoot));

        // OddProject may only have 2 fields:
        // - Engine: RootPath (string) with path to engine root relative to the OddProject.yaml
        // - Runtime: RootPath (string) with path to runtime project root relative to the OddProject.yaml
        m_Config["Engine"] = std::move(engineConf);

        if (runtimeConf)
        {
            m_Config["Runtime"] = std::move(runtimeConf);
        }
    }

    ConfigRegistry::~ConfigRegistry()
    {
        GConfig = nullptr;
    }

    YAML::Node ConfigRegistry::LoadConfigFile(const std::string& file)
    {
        const fs::path currentPath = file;
        if (!fs::exists(currentPath))
        {
            return {};
        }

        YAML::Node outConfig = YAML::LoadFile(file);
        if (auto includes = std::as_const(outConfig)["Include"])
        {
            const fs::path parentPath = currentPath.parent_path();
            oddValidate(fs::exists(parentPath));

            for (const auto& include : includes)
            {
                const std::string includeString = include.as<std::string>();
                fs::path          otherPath = parentPath / includeString;
                otherPath += ".yaml";

                YAML::Node loadedInclude = LoadConfigFile(otherPath.string());
                outConfig[include.as<std::string>()] = loadedInclude;
            }

            outConfig.remove(includes);
        }

        return outConfig;
    }

    std::optional<YAML::Node> ConfigRegistry::GetWithDottedPath(std::string_view path) const
    {
        // Trim path
        path = TrimStringView(path);
        const auto tokens = SplitDottedPath(path);

        YAML::Node current = m_Config;
        for (const auto& token : tokens)
        {
            YAML::Node child = std::as_const(current)[token];
            if (!child.IsDefined())
                return std::nullopt;
            current.reset(child);
        }

        return current;
    }

    void Internal::InitializeConfig()
    {
        oddValidate(!GConfig);
        GConfig = new ConfigRegistry;
    }

    void Internal::ShutdownConfig()
    {
        delete GConfig;
    }

} // namespace Odd