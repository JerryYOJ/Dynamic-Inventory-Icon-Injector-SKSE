#pragma once

#include "FormTypes.h"

namespace Config {
    inline const EnumMap<RE::BIPED_MODEL::ArmorType> WeightClassMap = {
        {"Light", RE::BIPED_MODEL::ArmorType::kLightArmor},
        {"Heavy", RE::BIPED_MODEL::ArmorType::kHeavyArmor},
        {"Clothing", RE::BIPED_MODEL::ArmorType::kClothing},
    };
}
