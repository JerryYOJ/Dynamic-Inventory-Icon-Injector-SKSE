#pragma once

#include "Condition.h"

namespace Config {
    struct IconData {
        std::string source; // SWF path
        std::string label;  // Export label
        std::string replace; // Empty = append, or vanilla icon name to replace
    };

    class Rule {
    public:
        void AddCondition(std::unique_ptr<Condition> cond) {
            _conditions.push_back(std::move(cond));
        }

        void SetIcon(IconData icon) { _icon = std::move(icon); }

        const IconData &GetIcon() const { return _icon; }

        bool Match(RE::InventoryEntryData *entry) const {
            for (const auto &cond : _conditions) {
                if (!cond->Match(entry))
                    return false;
            }
            return true;
        }

    private:
        std::vector<std::unique_ptr<Condition>> _conditions;
        IconData _icon;
    };
}
