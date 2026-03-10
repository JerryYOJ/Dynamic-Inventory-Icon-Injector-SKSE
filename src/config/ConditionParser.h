#pragma once

#include "Rule.h"

#include <json/json.h>

namespace Config {
	class ConditionParser {
		friend class ConfigManager;
	public:
		ConditionParser() = delete;

		static Rule ParseRule(const Json::Value &a_rule);

	protected:
		// Item-level condition builders
		using Builder = DIII::ConditionBuilder;

		static std::map<std::string, Builder, CaseInsensitiveCompare> BuilderMap;

		static std::unique_ptr<Condition> BuildConditionPerk(const Json::Value& val, RE::FormType);
		static std::unique_ptr<Condition> BuildFormType(const Json::Value &val, RE::FormType);
		static std::unique_ptr<Condition> BuildFormId(const Json::Value &val, RE::FormType);
		static std::unique_ptr<Condition> BuildKeywords(const Json::Value &val, RE::FormType);
		static std::unique_ptr<Condition> BuildBoolProperty(const Json::Value &val, bool (*getter)(RE::InventoryEntryData *));
		static std::unique_ptr<Condition> BuildNumberProperty(const Json::Value &val, float (*getter)(RE::InventoryEntryData *));
		static std::unique_ptr<Condition> BuildNot(const Json::Value &val, RE::FormType formType);
		static std::unique_ptr<Condition> BuildMagicEffect(const Json::Value& val, RE::FormType);
		static std::unique_ptr<Condition> BuildEnchantment(const Json::Value &val, RE::FormType);
		static std::unique_ptr<Condition> BuildPoison(const Json::Value &val, RE::FormType);
		static std::unique_ptr<Condition> BuildTempered(const Json::Value &val, RE::FormType);

		// Effect-level matcher builders
		static std::unique_ptr<EffectMatcher>
		BuildEffectMatcher(const std::string &name, const Json::Value &val);

		// Getter functions for item properties
		static bool IsStolen(RE::InventoryEntryData *entry);
		static bool IsFavorited(RE::InventoryEntryData *entry);
		static bool IsQuestItem(RE::InventoryEntryData *entry);
		static float GetGoldValue(RE::InventoryEntryData *entry);
		static float GetWeight(RE::InventoryEntryData *entry);
		static float GetBaseDamage(RE::InventoryEntryData *entry);
		static float GetSpeed(RE::InventoryEntryData *entry);
		static float GetReach(RE::InventoryEntryData *entry);
		static float GetStagger(RE::InventoryEntryData *entry);
		static float GetCritDamage(RE::InventoryEntryData *entry);
		static float GetArmorRating(RE::InventoryEntryData *entry);

		// Getter functions for effect properties
		static float GetMagnitude(RE::Effect *effect);
		static float GetDuration(RE::Effect *effect);
		static float GetArea(RE::Effect *effect);
	};
}
