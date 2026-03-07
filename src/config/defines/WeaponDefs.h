#pragma once

#include "FormTypes.h"

namespace Config {
    inline const EnumMap<RE::WEAPON_TYPE> WeaponTypeMap = {
        {"HandToHandMelee", RE::WEAPON_TYPE::kHandToHandMelee},
        {"OneHandSword", RE::WEAPON_TYPE::kOneHandSword},
        {"OneHandDagger", RE::WEAPON_TYPE::kOneHandDagger},
        {"OneHandAxe", RE::WEAPON_TYPE::kOneHandAxe},
        {"OneHandMace", RE::WEAPON_TYPE::kOneHandMace},
        {"TwoHandSword", RE::WEAPON_TYPE::kTwoHandSword},
        {"TwoHandAxe", RE::WEAPON_TYPE::kTwoHandAxe},
        {"Bow", RE::WEAPON_TYPE::kBow},
        {"Staff", RE::WEAPON_TYPE::kStaff},
        {"Crossbow", RE::WEAPON_TYPE::kCrossbow},
    };
}
