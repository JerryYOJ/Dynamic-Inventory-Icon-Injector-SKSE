#pragma once

#include "Rule.h"


namespace Config {
    class ConfigManager : public SINGLETON<ConfigManager> {
        friend SINGLETON;

    public:
        void LoadConfigs();

        // Returns all matching icons for an item
        std::vector<const IconData *> GetIcons(RE::InventoryEntryData *entry) const;

        // Returns SWF load requests grouped by source path (for ImportData::ImportResources)
        std::unordered_map<std::string, std::unordered_set<std::string>> GetLoadedIcons() const;

        std::size_t GetRuleCount() const { return _rules.size(); }

    private:
        ConfigManager() = default;

        void LoadFile(const std::filesystem::path &path);

        std::vector<Rule> _rules;
    };
}
