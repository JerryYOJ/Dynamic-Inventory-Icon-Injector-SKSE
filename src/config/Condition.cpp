#include "Condition.h"

namespace Config {
	inline static std::vector<RE::BIPED_MODEL::BipedObjectSlot> PartMaskPrecedence = {
		RE::BIPED_MODEL::BipedObjectSlot::kBody,
		RE::BIPED_MODEL::BipedObjectSlot::kHair,
		RE::BIPED_MODEL::BipedObjectSlot::kHands,
		RE::BIPED_MODEL::BipedObjectSlot::kForearms,
		RE::BIPED_MODEL::BipedObjectSlot::kFeet,
		RE::BIPED_MODEL::BipedObjectSlot::kCalves,
		RE::BIPED_MODEL::BipedObjectSlot::kShield,
		RE::BIPED_MODEL::BipedObjectSlot::kAmulet,
		RE::BIPED_MODEL::BipedObjectSlot::kRing,
		RE::BIPED_MODEL::BipedObjectSlot::kLongHair,
		RE::BIPED_MODEL::BipedObjectSlot::kEars,
		RE::BIPED_MODEL::BipedObjectSlot::kHead,
		RE::BIPED_MODEL::BipedObjectSlot::kCirclet,
		RE::BIPED_MODEL::BipedObjectSlot::kTail,
		static_cast<RE::BIPED_MODEL::BipedObjectSlot>(1 << 14),
		static_cast<RE::BIPED_MODEL::BipedObjectSlot>(1 << 15),
		static_cast<RE::BIPED_MODEL::BipedObjectSlot>(1 << 16),
		static_cast<RE::BIPED_MODEL::BipedObjectSlot>(1 << 17),
		static_cast<RE::BIPED_MODEL::BipedObjectSlot>(1 << 18),
		static_cast<RE::BIPED_MODEL::BipedObjectSlot>(1 << 19),
		RE::BIPED_MODEL::BipedObjectSlot::kDecapitateHead,
		RE::BIPED_MODEL::BipedObjectSlot::kDecapitate,
		static_cast<RE::BIPED_MODEL::BipedObjectSlot>(1 << 22),
		static_cast<RE::BIPED_MODEL::BipedObjectSlot>(1 << 23),
		static_cast<RE::BIPED_MODEL::BipedObjectSlot>(1 << 24),
		static_cast<RE::BIPED_MODEL::BipedObjectSlot>(1 << 25),
		static_cast<RE::BIPED_MODEL::BipedObjectSlot>(1 << 26),
		static_cast<RE::BIPED_MODEL::BipedObjectSlot>(1 << 27),
		static_cast<RE::BIPED_MODEL::BipedObjectSlot>(1 << 28),
		static_cast<RE::BIPED_MODEL::BipedObjectSlot>(1 << 29),
		static_cast<RE::BIPED_MODEL::BipedObjectSlot>(1 << 30),
		RE::BIPED_MODEL::BipedObjectSlot::kFX01,
	};

	// ========================================================================
	// HasKeywordCondition
	// ========================================================================

	bool HasKeywordCondition::Match(RE::InventoryEntryData *entry) const {
		if (!entry || !entry->object || !_keyword)
			return false;

		auto *keywordForm = entry->object->As<RE::BGSKeywordForm>();
		if (!keywordForm)
			return false;

		return keywordForm->HasKeyword(_keyword);
	}

	// ========================================================================
	// MainPartCondition
	// ========================================================================

	bool MainPartCondition::Match(RE::InventoryEntryData *entry) const {
		if (!entry || !entry->object)
			return false;

		auto *armor = entry->object->As<RE::TESObjectARMO>();
		if (!armor)
			return false;

		auto partMask = static_cast<std::uint32_t>(armor->GetSlotMask());

		if ((partMask & _slot) == 0)
			return false;

		// Determine main part by precedence
		std::uint32_t mainPart = 0;
		for (auto slot : PartMaskPrecedence) {
			auto val = static_cast<std::uint32_t>(slot);
			if (partMask & val) {
				mainPart = val;
				break;
			}
		}

		return mainPart != 0 && _slot == mainPart;
	}

	// ========================================================================
	// Two Helpers
	// ========================================================================

	static bool MatchEffects(const std::vector<std::unique_ptr<EffectMatcher>>& matchers, const RE::BSTArray<RE::Effect*>& effects) {
		if (matchers.empty())
			return true; // {} = just "is xxx"

		for (auto& effect : effects) {
			if (!effect)
				continue;

			bool allMatch =
				std::all_of(matchers.begin(), matchers.end(),
					[effect](const auto& m) { return m->Match(effect); });

			if (allMatch)
				return true;
		}
		return false;
	}

	static RE::EnchantmentItem* GetEnchantment(RE::InventoryEntryData* entry) {
		if (!entry || !entry->object)
			return nullptr;

		return entry->GetEnchantment();
	}

	// ========================================================================
	// MagicItemCondition
	// ========================================================================

	bool MagicItemCondition::Match(RE::InventoryEntryData *entry) const {
		if (!entry || !entry->GetObject())
			return false;
		
		auto* item = entry->GetObject()->As<RE::MagicItem>();
		if(!item)
			return false;

		return MatchEffects(_matchers, item->effects);
	}

	// ========================================================================
	// EnchantmentCondition
	// ========================================================================
	
	bool EnchantmentCondition::Match(RE::InventoryEntryData *entry) const {
		auto *enchantment = GetEnchantment(entry);
		if (!enchantment)
			return false;
		
		return MatchEffects(_matchers, enchantment->effects);
	}

	// ========================================================================
	// PoisonCondition
	// ========================================================================

	bool PoisonCondition::Match(RE::InventoryEntryData *entry) const {
		if (!entry || !entry->extraLists)
			return false;
		
		for (auto *extraList : *entry->extraLists) {
			if (!extraList)
				continue;

			auto *extraPoison = extraList->GetByType<RE::ExtraPoison>();
			if (!extraPoison || !extraPoison->poison)
				continue;

			return MatchEffects(_matchers, extraPoison->poison->effects);
		}
		return false;
	}

	// ========================================================================
	// TemperedCondition
	// ========================================================================

	bool TemperedCondition::Match(RE::InventoryEntryData *entry) const {
		if (!entry || !entry->extraLists)
			return false;
		
		for (auto *extraList : *entry->extraLists) {
			if (!extraList)
				continue;

			auto *extraHealth = extraList->GetByType<RE::ExtraHealth>();
			if (!extraHealth)
				continue;

			float health = extraHealth->health;
			if (_min.has_value() && health < _min.value())
				return false;
			if (_max.has_value() && health > _max.value())
				return false;
			return true;
		}
		return false;
	}
}
