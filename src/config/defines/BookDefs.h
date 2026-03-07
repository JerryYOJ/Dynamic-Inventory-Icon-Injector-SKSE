#pragma once

#include "FormTypes.h"

namespace Config {
    inline const EnumMap<RE::ActorValue> TeachesSkillMap = {
        {"OneHanded", RE::ActorValue::kOneHanded},
        {"TwoHanded", RE::ActorValue::kTwoHanded},
        {"Marksman", RE::ActorValue::kArchery},
        {"Block", RE::ActorValue::kBlock},
        {"Smithing", RE::ActorValue::kSmithing},
        {"HeavyArmor", RE::ActorValue::kHeavyArmor},
        {"LightArmor", RE::ActorValue::kLightArmor},
        {"Pickpocket", RE::ActorValue::kPickpocket},
        {"Lockpicking", RE::ActorValue::kLockpicking},
        {"Sneak", RE::ActorValue::kSneak},
        {"Alchemy", RE::ActorValue::kAlchemy},
        {"Speechcraft", RE::ActorValue::kSpeech},
        {"Alteration", RE::ActorValue::kAlteration},
        {"Conjuration", RE::ActorValue::kConjuration},
        {"Destruction", RE::ActorValue::kDestruction},
        {"Illusion", RE::ActorValue::kIllusion},
        {"Restoration", RE::ActorValue::kRestoration},
        {"Enchanting", RE::ActorValue::kEnchanting},
    };
}
