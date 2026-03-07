#pragma once

#include "FormTypes.h"

namespace Config {
    inline const EnumMap<RE::ActorValue> SchoolMap = {
        {"Alteration", RE::ActorValue::kAlteration},
        {"Conjuration", RE::ActorValue::kConjuration},
        {"Destruction", RE::ActorValue::kDestruction},
        {"Illusion", RE::ActorValue::kIllusion},
        {"Restoration", RE::ActorValue::kRestoration},
    };

    inline const EnumMap<RE::ActorValue> ResistanceMap = {
        {"DamageResist", RE::ActorValue::kDamageResist},
        {"PoisonResist", RE::ActorValue::kPoisonResist},
        {"FireResist", RE::ActorValue::kResistFire},
        {"ElectricResist", RE::ActorValue::kResistShock},
        {"FrostResist", RE::ActorValue::kResistFrost},
        {"MagicResist", RE::ActorValue::kResistMagic},
        {"DiseaseResist", RE::ActorValue::kResistDisease},
    };

    inline const EnumMap<RE::ActorValue> ActorValueMap = {
        {"Health", RE::ActorValue::kHealth},
        {"Magicka", RE::ActorValue::kMagicka},
        {"Stamina", RE::ActorValue::kStamina},
        {"HealRate", RE::ActorValue::kHealRate},
        {"MagickaRate", RE::ActorValue::kMagickaRate},
        {"StaminaRate", RE::ActorValue::kStaminaRate},
        {"SpeedMult", RE::ActorValue::kSpeedMult},
        {"CarryWeight", RE::ActorValue::kCarryWeight},
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

    inline const EnumMap<RE::MagicSystem::Delivery> DeliveryTypeMap = {
        {"Self", RE::MagicSystem::Delivery::kSelf},
        {"Touch", RE::MagicSystem::Delivery::kTouch},
        {"Aimed", RE::MagicSystem::Delivery::kAimed},
        {"TargetActor", RE::MagicSystem::Delivery::kTargetActor},
        {"TargetLocation", RE::MagicSystem::Delivery::kTargetLocation},
    };

    inline const EnumMap<RE::MagicSystem::CastingType> CastingTypeMap = {
        {"ConstantEffect", RE::MagicSystem::CastingType::kConstantEffect},
        {"FireAndForget", RE::MagicSystem::CastingType::kFireAndForget},
        {"Concentration", RE::MagicSystem::CastingType::kConcentration},
        {"Scroll", RE::MagicSystem::CastingType::kScroll},
    };

    inline const EnumMap<RE::EffectSetting::EffectSettingData::Flag> EffectFlagMap =
        {
            {"Hostile", RE::EffectSetting::EffectSettingData::Flag::kHostile},
            {"Recover", RE::EffectSetting::EffectSettingData::Flag::kRecover},
            {"Detrimental",
             RE::EffectSetting::EffectSettingData::Flag::kDetrimental},
            {"NoDuration", RE::EffectSetting::EffectSettingData::Flag::kNoDuration},
            {"NoArea", RE::EffectSetting::EffectSettingData::Flag::kNoArea},
            {"FXPersist", RE::EffectSetting::EffectSettingData::Flag::kFXPersist},
            {"GoryVisuals",
             RE::EffectSetting::EffectSettingData::Flag::kGoryVisuals},
            {"HideInUI", RE::EffectSetting::EffectSettingData::Flag::kHideInUI},
            {"Painless", RE::EffectSetting::EffectSettingData::Flag::kPainless},
            {"NoHitEffect",
             RE::EffectSetting::EffectSettingData::Flag::kNoHitEffect},
            {"NoDeathDispel",
             RE::EffectSetting::EffectSettingData::Flag::kNoDeathDispel},
            {"PowerAffectsMagnitude",
             RE::EffectSetting::EffectSettingData::Flag::kPowerAffectsMagnitude},
            {"PowerAffectsDuration",
             RE::EffectSetting::EffectSettingData::Flag::kPowerAffectsDuration},
    };

    inline const EnumMap<RE::EffectArchetype> ArchetypeMap = {
        {"ValueModifier", RE::EffectArchetype::kValueModifier},
        {"Script", RE::EffectArchetype::kScript},
        {"Dispel", RE::EffectArchetype::kDispel},
        {"CureDisease", RE::EffectArchetype::kCureDisease},
        {"Absorb", RE::EffectArchetype::kAbsorb},
        {"DualValueModifier", RE::EffectArchetype::kDualValueModifier},
        {"Calm", RE::EffectArchetype::kCalm},
        {"Demoralize", RE::EffectArchetype::kDemoralize},
        {"Frenzy", RE::EffectArchetype::kFrenzy},
        {"Disarm", RE::EffectArchetype::kDisarm},
        {"CommandSummoned", RE::EffectArchetype::kCommandSummoned},
        {"Invisibility", RE::EffectArchetype::kInvisibility},
        {"Light", RE::EffectArchetype::kLight},
        {"Lock", RE::EffectArchetype::kLock},
        {"Open", RE::EffectArchetype::kOpen},
        {"BoundWeapon", RE::EffectArchetype::kBoundWeapon},
        {"SummonCreature", RE::EffectArchetype::kSummonCreature},
        {"DetectLife", RE::EffectArchetype::kDetectLife},
        {"Telekinesis", RE::EffectArchetype::kTelekinesis},
        {"Paralysis", RE::EffectArchetype::kParalysis},
        {"Reanimate", RE::EffectArchetype::kReanimate},
        {"SoulTrap", RE::EffectArchetype::kSoulTrap},
        {"TurnUndead", RE::EffectArchetype::kTurnUndead},
        {"Guide", RE::EffectArchetype::kGuide},
        {"Cloak", RE::EffectArchetype::kCloak},
        {"SlowTime", RE::EffectArchetype::kSlowTime},
        {"Rally", RE::EffectArchetype::kRally},
        {"EnhanceWeapon", RE::EffectArchetype::kEnhanceWeapon},
        {"SpawnHazard", RE::EffectArchetype::kSpawnHazard},
        {"Etherealize", RE::EffectArchetype::kEtherealize},
        {"Banish", RE::EffectArchetype::kBanish},
        {"Disguise", RE::EffectArchetype::kDisguise},
        {"PeakValueModifier", RE::EffectArchetype::kPeakValueModifier},
    };
}
