#include "ConditionParser.h"

#include "ParseUtil.h"
#include "defines/ArmorDefs.h"
#include "defines/BookDefs.h"
#include "defines/FormTypes.h"
#include "defines/MagicDefs.h"
#include "defines/PotionDefs.h"
#include "defines/SoulGemDefs.h"
#include "defines/WeaponDefs.h"

namespace Config {
	// ========================================================================
	// Item-level getter functions
	// ========================================================================

	bool ConditionParser::IsStolen(RE::InventoryEntryData *entry) {
		return entry && entry->IsOwnedBy(RE::PlayerCharacter::GetSingleton(), true);
	}

	bool ConditionParser::IsFavorited(RE::InventoryEntryData *entry) {
		if (!entry || !entry->extraLists)
			return false;
		return entry->IsFavorited();
	}

	bool ConditionParser::IsQuestItem(RE::InventoryEntryData *entry) {
		if (!entry || !entry->extraLists)
			return false;
		return entry->IsQuestObject();
	}

	float ConditionParser::GetGoldValue(RE::InventoryEntryData *entry) {
		return entry ? static_cast<float>(entry->GetValue()) : 0.f;
	}

	float ConditionParser::GetWeight(RE::InventoryEntryData *entry) {
		return (entry && entry->object) ? entry->object->GetWeight() : 0.f;
	}

	float ConditionParser::GetBaseDamage(RE::InventoryEntryData *entry) {
		if (!entry || !entry->object)
			return 0.f;
		auto *weap = entry->object->As<RE::TESObjectWEAP>();
		return weap ? static_cast<float>(weap->GetAttackDamage()) : 0.f;
	}

	float ConditionParser::GetSpeed(RE::InventoryEntryData *entry) {
		if (!entry || !entry->object)
			return 0.f;
		auto *weap = entry->object->As<RE::TESObjectWEAP>();
		return weap ? weap->GetSpeed() : 0.f;
	}

	float ConditionParser::GetReach(RE::InventoryEntryData *entry) {
		if (!entry || !entry->object)
			return 0.f;
		auto *weap = entry->object->As<RE::TESObjectWEAP>();
		return weap ? weap->GetReach() : 0.f;
	}

	float ConditionParser::GetStagger(RE::InventoryEntryData *entry) {
		if (!entry || !entry->object)
			return 0.f;
		auto *weap = entry->object->As<RE::TESObjectWEAP>();
		return weap ? weap->GetStagger() : 0.f;
	}

	float ConditionParser::GetCritDamage(RE::InventoryEntryData *entry) {
		if (!entry || !entry->object)
			return 0.f;
		auto *weap = entry->object->As<RE::TESObjectWEAP>();
		return weap ? static_cast<float>(weap->GetCritDamage()) : 0.f;
	}

	float ConditionParser::GetArmorRating(RE::InventoryEntryData *entry) {
		if (!entry || !entry->object)
			return 0.f;
		auto *armor = entry->object->As<RE::TESObjectARMO>();
		return armor ? static_cast<float>(armor->GetArmorRating()) : 0.f;
	}

	// ========================================================================
	// Effect-level getter functions
	// ========================================================================

	float ConditionParser::GetMagnitude(RE::Effect *effect) {
		return effect ? effect->effectItem.magnitude : 0.f;
	}

	float ConditionParser::GetDuration(RE::Effect *effect) {
		return effect ? static_cast<float>(effect->effectItem.duration) : 0.f;
	}

	float ConditionParser::GetArea(RE::Effect *effect) {
		return effect ? static_cast<float>(effect->effectItem.area) : 0.f;
	}

	// ========================================================================
	// Builder functions — Player Level
	// ========================================================================

	std::unique_ptr<Condition> ConditionParser::BuildConditionPerk(const Json::Value& val, RE::FormType) {
		if (val.isString()) {
			auto id = ParseUtil::ParseFormID(val.asString());
			if (id == 0)
				return nullptr;
			auto* perk = RE::TESForm::LookupByID<RE::BGSPerk>(id);
			if (!perk) {
				logger::warn("conditionPerk: FormID {:08X} is not a valid Perk", id);
				return nullptr;
			}
			return std::make_unique<PerkConditionCondition>(perk);
		}
		else if (val.isArray()) {
			auto anyOf = std::make_unique<AnyOfCondition>();

			for (const auto& elem : val) {
				if (elem.isString()) {
					auto id = ParseUtil::ParseFormID(elem.asString());
					if (id == 0)
						return nullptr;
					auto* perk = RE::TESForm::LookupByID<RE::BGSPerk>(id);
					if (!perk) {
						logger::warn("conditionPerk: FormID {:08X} is not a valid Perk", id);
						return nullptr;
					}

					if (perk)
						anyOf->Add(std::make_unique<PerkConditionCondition>(perk));
				}
			}

			return anyOf;
		}
		return nullptr;
	}

	// ========================================================================
	// Builder functions — Item level
	// ========================================================================

	std::unique_ptr<Condition>
	ConditionParser::BuildFormType(const Json::Value &val, RE::FormType) {
		if (!val.isString())
			return nullptr;

		RE::FormType formType = RE::FormType::None;
		if (TryGetEnum(FormTypeMap, val.asString(), formType)) {
			return std::make_unique<ExactMatch<RE::FormType>>(
					formType, [](RE::InventoryEntryData *e) -> RE::FormType {
						return (e && e->object) ? e->object->GetFormType() : RE::FormType::None;
					});
		}
		return nullptr;
	}

	std::unique_ptr<Condition> ConditionParser::BuildFormId(const Json::Value &val, RE::FormType) {
		auto ids = ParseUtil::ParseFormIDArray(val);
		if (ids.empty())
			return nullptr;
		return std::make_unique<FormIdCondition>(std::move(ids));
	}

	std::unique_ptr<Condition>
	ConditionParser::BuildKeywords(const Json::Value &val, RE::FormType) {
		auto resolveKeyword = [](const std::string &name) -> RE::BGSKeyword * {
			auto form = RE::TESForm::LookupByEditorID(name);
			return form ? form->As<RE::BGSKeyword>() : nullptr;
		};

		if (val.isString()) {
			auto *kw = resolveKeyword(val.asString());
			if (kw)
				return std::make_unique<HasKeywordCondition>(kw);
		} else if (val.isArray()) {
			// Multiple keywords = AND (item must have all of them)
			// Currently we only add the first; caller handles adding multiple
			// by iterating the array in ParseRule
			for (const auto &elem : val) {
				if (elem.isString()) {
					auto *kw = resolveKeyword(elem.asString());
					if (kw)
						return std::make_unique<HasKeywordCondition>(kw);
				}
			}
		}
		return nullptr;
	}

	std::unique_ptr<Condition>
	ConditionParser::BuildBoolProperty(const Json::Value &val, bool (*getter)(RE::InventoryEntryData *)) {
		if (!val.isBool())
			return nullptr;
		return std::make_unique<BoolCondition>(val.asBool(), getter);
	}

	std::unique_ptr<Condition> ConditionParser::BuildNumberProperty(
			const Json::Value &val, float (*getter)(RE::InventoryEntryData *)
	) {
		if (val.isNumeric()) {
			return std::make_unique<RangeCondition>(static_cast<float>(val.asDouble()), static_cast<float>(val.asDouble()), getter);
		} else if (val.isObject()) {
			std::optional<float> min, max;
			if (val["min"].isNumeric())
				min = static_cast<float>(val["min"].asDouble());
			if (val["max"].isNumeric())
				max = static_cast<float>(val["max"].asDouble());
			if (min.has_value() || max.has_value()) {
				return std::make_unique<RangeCondition>(min, max, getter);
			}
		}
		return nullptr;
	}

	std::unique_ptr<Condition> ConditionParser::BuildNot(const Json::Value &val, RE::FormType formType) {
		if (!val.isObject())
			return nullptr;

		// Parse the inner object as conditions, combine with an implicit AND
		// then negate the whole thing
		std::vector<std::unique_ptr<Condition>> innerConds;

		for (const auto &name : val.getMemberNames()) {
			if (name.empty())
				continue;

			if (auto it = BuilderMap.find(name); it != BuilderMap.end()) {
				auto cond = it->second(val[name], formType);
				if (cond)
					innerConds.push_back(std::move(cond));
			}
		}

		if (innerConds.empty())
			return nullptr;

		if (innerConds.size() == 1) {
			return std::make_unique<NotCondition>(std::move(innerConds[0]));
		}

		// Multiple inner conditions -> AND them, then negate
		// We create a temporary rule-like wrapper
		// Actually, we just negate each individually and combine with AnyOf (De
		// Morgan) NOT(A AND B) = NOT A OR NOT B But the user likely wants NOT(A AND
		// B), so we build a custom condition:
		struct AllOfCondition final : public Condition {
			std::vector<std::unique_ptr<Condition>> conditions;
			bool Match(RE::InventoryEntryData *entry) const override {
				for (const auto &c : conditions) {
					if (!c->Match(entry))
						return false;
				}
				return true;
			}
		};

		auto allOf = std::make_unique<AllOfCondition>();
		allOf->conditions = std::move(innerConds);
		return std::make_unique<NotCondition>(std::move(allOf));
	}

	std::unique_ptr<Condition> ConditionParser::BuildMagicEffect(const Json::Value& val, RE::FormType) {
		auto cond = std::make_unique<MagicItemCondition>();
		if (val.isObject()) {
			for (const auto& name : val.getMemberNames()) {
				auto matcher = BuildEffectMatcher(name, val[name]);
				if (matcher)
					cond->AddMatcher(std::move(matcher));
			}
		}
		return cond;
	}

	std::unique_ptr<Condition> ConditionParser::BuildEnchantment(const Json::Value &val, RE::FormType type) {
		auto cond = std::make_unique<AllOfCondition>();

		if (val.isObject()) {
			auto enchCond = std::make_unique<EnchantmentCondition>();

			if (val.isMember("isKnown") && val["isKnown"].isBool()) {
				auto knownCond = std::make_unique<BoolCondition>(
					val["isKnown"].asBool(), [](RE::InventoryEntryData* e) -> bool {
						if (!e) return false;
						auto* ench = e->GetEnchantment();
						if (!ench) return false;
						if (RE::TESDataHandler::GetSingleton()->IsGeneratedID(ench->GetFormID())) return true;
						auto* base = ench->data.baseEnchantment
							? ench->data.baseEnchantment : ench;
						return base->GetKnown();
					});
				cond->Add(std::move(knownCond));
			}
			if (val.isMember("magicEffect") && val["magicEffect"].isObject()) {
				
				auto&& mgef = val["magicEffect"];

				for (const auto& name : mgef.getMemberNames()) {
					auto matcher = BuildEffectMatcher(name, mgef[name]);
					if (matcher)
						enchCond->AddMatcher(std::move(matcher));
				}
			}

			cond->Add(std::move(enchCond));
		}

		return cond;
	}

	std::unique_ptr<Condition> ConditionParser::BuildPoison(const Json::Value &val, RE::FormType type) {
		auto cond = std::make_unique<PoisonCondition>();

		if (val.isObject()) {
			
			if (val.isMember("magicEffect") && val["magicEffect"].isObject()) {
				//auto enchCond = std::make_unique<PoisonCondition>();

				auto&& mgef = val["magicEffect"];
				for (const auto& name : mgef.getMemberNames()) {
					auto matcher = BuildEffectMatcher(name, mgef[name]);
					if (matcher)
						cond->AddMatcher(std::move(matcher));
				}

				//cond->Add(std::move(enchCond));
			}
		}

		return cond;
	}

	std::unique_ptr<Condition>
	ConditionParser::BuildTempered(const Json::Value &val, RE::FormType) {
		std::optional<float> min, max;

		if (val.isObject()) {
			if (val["min"].isNumeric())
				min = static_cast<float>(val["min"].asDouble());
			if (val["max"].isNumeric())
				max = static_cast<float>(val["max"].asDouble());
		}

		return std::make_unique<TemperedCondition>(min, max);
	}

	// ========================================================================
	// Builder functions — Effect level
	// ========================================================================

	std::unique_ptr<EffectMatcher>
	ConditionParser::BuildEffectMatcher(const std::string &name, const Json::Value &val) {
		// effectId
		if (name == "effectId") {
			auto ids = ParseUtil::ParseFormIDArray(val);
			if (!ids.empty())
				return std::make_unique<EffectFormIdMatch>(std::move(ids));
			return nullptr;
		}

		// school
		if (name == "school") {
			RE::ActorValue school = RE::ActorValue::kNone;
			if (val.isString() && TryGetEnum(SchoolMap, val.asString(), school)) {
				return std::make_unique<EffectExactMatch<RE::ActorValue>>(
						school, [](RE::Effect *e) -> RE::ActorValue {
							return (e && e->baseEffect) ? e->baseEffect->GetMagickSkill() : RE::ActorValue::kNone;
						});
			}
			return nullptr;
		}

		// magnitude
		if (name == "magnitude") {
			if (val.isNumeric()) {
				float v = static_cast<float>(val.asDouble());
				return std::make_unique<EffectRangeMatch>(v, v, GetMagnitude);
			}
			if (val.isObject()) {
				std::optional<float> min, max;
				if (val["min"].isNumeric())
					min = static_cast<float>(val["min"].asDouble());
				if (val["max"].isNumeric())
					max = static_cast<float>(val["max"].asDouble());
				return std::make_unique<EffectRangeMatch>(min, max, GetMagnitude);
			}
			return nullptr;
		}

		// duration
		if (name == "duration") {
			if (val.isNumeric()) {
				float v = static_cast<float>(val.asDouble());
				return std::make_unique<EffectRangeMatch>(v, v, GetDuration);
			}
			if (val.isObject()) {
				std::optional<float> min, max;
				if (val["min"].isNumeric())
					min = static_cast<float>(val["min"].asDouble());
				if (val["max"].isNumeric())
					max = static_cast<float>(val["max"].asDouble());
				return std::make_unique<EffectRangeMatch>(min, max, GetDuration);
			}
			return nullptr;
		}

		// area
		if (name == "area") {
			if (val.isNumeric()) {
				float v = static_cast<float>(val.asDouble());
				return std::make_unique<EffectRangeMatch>(v, v, GetArea);
			}
			if (val.isObject()) {
				std::optional<float> min, max;
				if (val["min"].isNumeric())
					min = static_cast<float>(val["min"].asDouble());
				if (val["max"].isNumeric())
					max = static_cast<float>(val["max"].asDouble());
				return std::make_unique<EffectRangeMatch>(min, max, GetArea);
			}
			return nullptr;
		}

		// archetype
		if (name == "archetype") {
			RE::EffectArchetype archetype{};
			if (val.isString() && TryGetEnum(ArchetypeMap, val.asString(), archetype)) {
				return std::make_unique<EffectExactMatch<RE::EffectArchetype>>(
						archetype, [](RE::Effect *e) -> RE::EffectArchetype {
							return (e && e->baseEffect) ? e->baseEffect->GetArchetype() : RE::EffectArchetype::kNone;
						});
			}
			return nullptr;
		}

		// deliveryType
		if (name == "deliveryType") {
			RE::MagicSystem::Delivery delivery{};
			if (val.isString() &&
					TryGetEnum(DeliveryTypeMap, val.asString(), delivery)) {
				return std::make_unique<EffectExactMatch<RE::MagicSystem::Delivery>>(
						delivery, [](RE::Effect *e) -> RE::MagicSystem::Delivery {
							return (e && e->baseEffect) ? e->baseEffect->data.delivery : RE::MagicSystem::Delivery::kSelf;
						});
			}
			return nullptr;
		}

		// castType
		if (name == "castType") {
			RE::MagicSystem::CastingType casting{};
			if (val.isString() && TryGetEnum(CastingTypeMap, val.asString(), casting)) {
				return std::make_unique<EffectExactMatch<RE::MagicSystem::CastingType>>(
						casting, [](RE::Effect *e) -> RE::MagicSystem::CastingType {
							return (e && e->baseEffect) ? e->baseEffect->data.castingType : RE::MagicSystem::CastingType::kConstantEffect;
						});
			}
			return nullptr;
		}

		// primaryValue
		if (name == "primaryValue") {
			RE::ActorValue av = RE::ActorValue::kNone;
			if (val.isString() && TryGetEnum(ActorValueMap, val.asString(), av)) {
				return std::make_unique<EffectExactMatch<RE::ActorValue>>(
						av, [](RE::Effect *e) -> RE::ActorValue {
							return (e && e->baseEffect) ? e->baseEffect->data.primaryAV : RE::ActorValue::kNone;
						});
			}
			return nullptr;
		}

		// resistance
		if (name == "resistance") {
			RE::ActorValue resist = RE::ActorValue::kNone;
			if (val.isString() && TryGetEnum(ResistanceMap, val.asString(), resist)) {
				return std::make_unique<EffectExactMatch<RE::ActorValue>>(
						resist, [](RE::Effect *e) -> RE::ActorValue {
							return (e && e->baseEffect) ? e->baseEffect->data.resistVariable : RE::ActorValue::kNone;
						});
			}
			return nullptr;
		}

		// effectFlags
		if (name == "effectFlags") {
			std::uint32_t combinedFlags = 0;
			auto processFlag = [&](const Json::Value &v) {
				if (v.isString()) {
					RE::EffectSetting::EffectSettingData::Flag flag{};
					if (TryGetEnum(EffectFlagMap, v.asString(), flag)) {
						combinedFlags |= static_cast<std::uint32_t>(flag);
					}
				}
			};

			if (val.isString()) {
				processFlag(val);
			} else if (val.isArray()) {
				for (const auto &elem : val)
					processFlag(elem);
			}

			if (combinedFlags != 0) {
				return std::make_unique<EffectBitfieldMatch>(combinedFlags);
			}
			return nullptr;
		}

		// effectKeywords
		if (name == "effectKeywords") {
			auto resolveKw = [](const std::string &n) -> RE::BGSKeyword * {
				auto form = RE::TESForm::LookupByEditorID(n);
				return form ? form->As<RE::BGSKeyword>() : nullptr;
			};

			if (val.isString()) {
				auto *kw = resolveKw(val.asString());
				if (kw)
					return std::make_unique<EffectKeywordMatch>(kw);
			}
			// TODO: array of keywords for AND
			return nullptr;
		}

		// skillLevel
		if (name == "skillLevel") {
			if (val.isNumeric()) {
				float v = static_cast<float>(val.asDouble());
				return std::make_unique<EffectRangeMatch>(
						v, v, [](RE::Effect *e) -> float {
							return (e && e->baseEffect) ? static_cast<float>(e->baseEffect->GetMinimumSkillLevel()) : 0.f;
						});
			}
			if (val.isObject()) {
				std::optional<float> min, max;
				if (val["min"].isNumeric())
					min = static_cast<float>(val["min"].asDouble());
				if (val["max"].isNumeric())
					max = static_cast<float>(val["max"].asDouble());
				return std::make_unique<EffectRangeMatch>(
						min, max, [](RE::Effect *e) -> float {
							return (e && e->baseEffect) ? static_cast<float>(e->baseEffect->GetMinimumSkillLevel()) : 0.f;
						});
			}
			return nullptr;
		}

		logger::warn("Unknown effect matcher: {}", name);
		return nullptr;
	}

	// ========================================================================
	// Builder map
	// ========================================================================

	const std::map<std::string, ConditionParser::Builder, CaseInsensitiveCompare>
		ConditionParser::BuilderMap = {
			// Common
			{"formType", BuildFormType},
			{"formId", BuildFormId},
			{"keywords", BuildKeywords},
			{"isStolen",
				[](const Json::Value &v, RE::FormType) -> std::unique_ptr<Condition> {
					return BuildBoolProperty(v, IsStolen);
				}},
			{"isFavorited",
				[](const Json::Value &v, RE::FormType) -> std::unique_ptr<Condition> {
					return BuildBoolProperty(v, IsFavorited);
				}},
			{"isQuestItem",
				[](const Json::Value &v, RE::FormType) -> std::unique_ptr<Condition> {
					return BuildBoolProperty(v, IsQuestItem);
				}},
			{"goldValue",
				[](const Json::Value &v, RE::FormType) -> std::unique_ptr<Condition> {
					return BuildNumberProperty(v, GetGoldValue);
				}},
			{"weight",
				[](const Json::Value &v, RE::FormType) -> std::unique_ptr<Condition> {
					return BuildNumberProperty(v, GetWeight);
				}},
			{"conditionPerk", BuildConditionPerk},
			// Weapon
			{"weaponType",
				[](const Json::Value &v, RE::FormType) -> std::unique_ptr<Condition> {
					if (!v.isString())
						return nullptr;
					RE::WEAPON_TYPE wt{};
					if (TryGetEnum(WeaponTypeMap, v.asString(), wt)) {
						return std::make_unique<ExactMatch<RE::WEAPON_TYPE>>(
								wt, [](RE::InventoryEntryData *e) -> RE::WEAPON_TYPE {
									if (!e || !e->object)
										return RE::WEAPON_TYPE::kHandToHandMelee;
									auto *w = e->object->As<RE::TESObjectWEAP>();
									return w ? w->GetWeaponType() : RE::WEAPON_TYPE::kHandToHandMelee;
								});
					}
					return nullptr;
				}},
			{"baseDamage",
				[](const Json::Value &v, RE::FormType) -> std::unique_ptr<Condition> {
					return BuildNumberProperty(v, GetBaseDamage);
				}},
			{"speed",
				[](const Json::Value &v, RE::FormType) -> std::unique_ptr<Condition> {
					return BuildNumberProperty(v, GetSpeed);
				}},
			{"reach",
				[](const Json::Value &v, RE::FormType) -> std::unique_ptr<Condition> {
					return BuildNumberProperty(v, GetReach);
				}},
			{"stagger",
				[](const Json::Value &v, RE::FormType) -> std::unique_ptr<Condition> {
					return BuildNumberProperty(v, GetStagger);
				}},
			{"critDamage",
				[](const Json::Value &v, RE::FormType) -> std::unique_ptr<Condition> {
					return BuildNumberProperty(v, GetCritDamage);
				}},
			// Armor
			{"weightClass",
				[](const Json::Value &v, RE::FormType) -> std::unique_ptr<Condition> {
					if (!v.isString())
						return nullptr;
					RE::BIPED_MODEL::ArmorType at{};
					if (TryGetEnum(WeightClassMap, v.asString(), at)) {
						return std::make_unique<ExactMatch<RE::BIPED_MODEL::ArmorType>>(
								at,
								[](RE::InventoryEntryData *e) -> RE::BIPED_MODEL::ArmorType {
									if (!e || !e->object)
										return RE::BIPED_MODEL::ArmorType::kClothing;
									auto *a = e->object->As<RE::TESObjectARMO>();
									return a ? a->GetArmorType() : RE::BIPED_MODEL::ArmorType::kClothing;
								});
					}
					return nullptr;
				}},
			{"armorRating",
				[](const Json::Value &v, RE::FormType) -> std::unique_ptr<Condition> {
					return BuildNumberProperty(v, GetArmorRating);
				}},
			{"mainPart",
				[](const Json::Value &v, RE::FormType) -> std::unique_ptr<Condition> {
					if (!v.isNumeric())
						return nullptr;
					auto slot = static_cast<std::uint32_t>(v.asUInt());
					if (slot < 30 || slot > 61)
						return nullptr;
					return std::make_unique<MainPartCondition>(1u << (slot - 30));
				}},
			// Potion
			{"potionType",
				[](const Json::Value &v, RE::FormType) -> std::unique_ptr<Condition> {
					if (!v.isString())
						return nullptr;
					PotionType pt{};
					if (TryGetEnum(PotionTypeMap, v.asString(), pt)) {
						return std::make_unique<ExactMatch<PotionType>>(
								pt, [](RE::InventoryEntryData *e) -> PotionType {
									if (!e || !e->object)
										return PotionType::kMedicine;
									auto *p = e->object->As<RE::AlchemyItem>();
									if (!p)
										return PotionType::kMedicine;
									if (p->IsFood())
										return PotionType::kFood;
									if (p->IsPoison())
										return PotionType::kPoison;
									return PotionType::kMedicine;
								});
					}
					return nullptr;
				}},
			// SoulGem
			{"gemSize",
				[](const Json::Value &v, RE::FormType) -> std::unique_ptr<Condition> {
					if (!v.isString())
						return nullptr;
					RE::SOUL_LEVEL size{};
					if (TryGetEnum(GemSizeMap, v.asString(), size)) {
						return std::make_unique<ExactMatch<RE::SOUL_LEVEL>>(
								size, [](RE::InventoryEntryData *e) -> RE::SOUL_LEVEL {
									if (!e || !e->object)
										return RE::SOUL_LEVEL::kNone;
									auto *gem = e->object->As<RE::TESSoulGem>();
									return gem ? gem->GetMaximumCapacity()
														: RE::SOUL_LEVEL::kNone;
								});
					}
					return nullptr;
				}},
			{"soulSize",
				[](const Json::Value &v, RE::FormType) -> std::unique_ptr<Condition> {
					if (!v.isString())
						return nullptr;
					RE::SOUL_LEVEL size{};
					if (TryGetEnum(SoulSizeMap, v.asString(), size)) {
						return std::make_unique<ExactMatch<RE::SOUL_LEVEL>>(
								size, [](RE::InventoryEntryData *e) -> RE::SOUL_LEVEL {
									if (!e || !e->object)
										return RE::SOUL_LEVEL::kNone;
									return e->GetSoulLevel();
								});
					}
					return nullptr;
				}},
			// Book
			{"teachesSpell",
				[](const Json::Value &v, RE::FormType) -> std::unique_ptr<Condition> {
					if (v.isBool()) {
						return std::make_unique<BoolCondition>(
								v.asBool(), [](RE::InventoryEntryData *e) -> bool {
									if (!e || !e->object)
										return false;
									auto *book = e->object->As<RE::TESObjectBOOK>();
									return book ? book->TeachesSpell() : false;
								});
					}
					return nullptr;
				}},
			{"teachesSkill",
				[](const Json::Value &v, RE::FormType) -> std::unique_ptr<Condition> {
					if (!v.isString())
						return nullptr;
					RE::ActorValue skill = RE::ActorValue::kNone;
					if (TryGetEnum(TeachesSkillMap, v.asString(), skill)) {
						return std::make_unique<ExactMatch<RE::ActorValue>>(
								skill, [](RE::InventoryEntryData *e) -> RE::ActorValue {
									if (!e || !e->object)
										return RE::ActorValue::kNone;
									auto *book = e->object->As<RE::TESObjectBOOK>();
									return book ? book->GetSkill() : RE::ActorValue::kNone;
								});
					}
					return nullptr;
				}},
			{"isRead",
				[](const Json::Value &v, RE::FormType) -> std::unique_ptr<Condition> {
					if (!v.isBool())
						return nullptr;
					return std::make_unique<BoolCondition>(
							v.asBool(), [](RE::InventoryEntryData *e) -> bool {
								if (!e || !e->object)
									return false;
								auto *book = e->object->As<RE::TESObjectBOOK>();
								return book ? book->IsRead() : false;
							});
				}},
			// Object-field conditions
			{"enchantment", BuildEnchantment},
			{"poison", BuildPoison},
			{"tempered", BuildTempered},
			{"magicEffect",
				BuildMagicEffect},
			// Special
			{"not", BuildNot},
		};

	// ========================================================================
	// ParseRule — entry point
	// ========================================================================

	Rule ConditionParser::ParseRule(const Json::Value &a_rule) {
		Rule parsed;

		const Json::Value &icon = a_rule["icon"];
		const Json::Value &match = a_rule["match"];

		if (!icon.isObject() || !match.isObject()) {
			return parsed;
		}

		// Parse icon data
		IconData iconData;
		iconData.source = icon.get("source", "").asString();
		iconData.label = icon.get("label", "").asString();
		iconData.replace = icon.get("replace", "").asString();
		parsed.SetIcon(std::move(iconData));

		// Determine form type for context-sensitive fields
		RE::FormType formType = RE::FormType::None;
		if (match["formType"].isString()) {
			TryGetEnum(FormTypeMap, match["formType"].asString(), formType);
		}

		// Parse each match field
		for (const auto &name : match.getMemberNames()) {
			if (name.empty() || name[0] == '$')
				continue;

			// Handle keywords array specially — each keyword is a separate condition
			// (AND)
			if (name == "keywords" && match[name].isArray()) {
				for (const auto &elem : match[name]) {
					Json::Value single = elem;
					auto cond = BuildKeywords(single, formType);
					if (cond)
						parsed.AddCondition(std::move(cond));
				}
				continue;
			}

			if (auto it = BuilderMap.find(name); it != BuilderMap.end()) {
				auto cond = it->second(match[name], formType);
				if (cond)
					parsed.AddCondition(std::move(cond));
			} else {
				logger::warn("Unknown match field: {}", name);
			}
		}

		return parsed;
	}
}
