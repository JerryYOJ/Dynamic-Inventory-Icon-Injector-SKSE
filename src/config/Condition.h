#pragma once

#include "EffectMatcher.h"

namespace Config {
// ========================================================================
// Base class — matches against an inventory entry
// ========================================================================

	class Condition {
	public:
		virtual ~Condition() = default;
		[[nodiscard]] virtual bool Match(RE::InventoryEntryData *entry) const = 0;
	};

	// ========================================================================
	// Generic matchers
	// ========================================================================

	template <typename T> class ExactMatch final : public Condition {
	public:
		using Getter = T (*)(RE::InventoryEntryData *);

		ExactMatch(T expected, Getter getter)
				: _expected(expected), _getter(getter) {}

		bool Match(RE::InventoryEntryData *entry) const override {
			return _getter(entry) == _expected;
		}

	private:
		T _expected;
		Getter _getter;
	};

	class FormIdCondition final : public Condition {
	public:
		FormIdCondition(std::vector<RE::FormID> ids) : _ids(std::move(ids)) {}

		bool Match(RE::InventoryEntryData *entry) const override {
			if (!entry || !entry->object)
				return false;
			return std::find(_ids.begin(), _ids.end(), entry->object->formID) !=
						_ids.end();
		}

	private:
		std::vector<RE::FormID> _ids;
	};

	class RangeCondition final : public Condition {
	public:
		using Getter = float (*)(RE::InventoryEntryData *);

		RangeCondition(std::optional<float> min, std::optional<float> max,
									Getter getter)
				: _min(min), _max(max), _getter(getter) {}

		bool Match(RE::InventoryEntryData *entry) const override {
			float value = _getter(entry);
			if (_min.has_value() && value < _min.value())
				return false;
			if (_max.has_value() && value > _max.value())
				return false;
			return true;
		}

	private:
		std::optional<float> _min, _max;
		Getter _getter;
	};

	class BoolCondition final : public Condition {
	public:
		using Getter = bool (*)(RE::InventoryEntryData *);

		BoolCondition(bool expected, Getter getter)
				: _expected(expected), _getter(getter) {}

		bool Match(RE::InventoryEntryData *entry) const override {
			return _getter(entry) == _expected;
		}

	private:
		bool _expected;
		Getter _getter;
	};

	class HasKeywordCondition final : public Condition {
	public:
		HasKeywordCondition(RE::BGSKeyword *keyword) : _keyword(keyword) {}

		bool Match(RE::InventoryEntryData *entry) const override;

	private:
		RE::BGSKeyword *_keyword;
	};

	class BitfieldCondition final : public Condition {
	public:
		using Getter = std::uint32_t (*)(RE::InventoryEntryData *);

		BitfieldCondition(std::uint32_t flags, Getter getter)
				: _flags(flags), _getter(getter) {}

		bool Match(RE::InventoryEntryData *entry) const override {
			return (_getter(entry) & _flags) != 0;
		}

	private:
		std::uint32_t _flags;
		Getter _getter;
	};

	class MainPartCondition final : public Condition {
	public:
		MainPartCondition(std::uint32_t slot) : _slot(slot) {}

		bool Match(RE::InventoryEntryData *entry) const override;

	private:
		std::uint32_t _slot;
	};

	// ========================================================================
	// Logical combinators
	// ========================================================================

	class NotCondition final : public Condition {
	public:
		NotCondition(std::unique_ptr<Condition> inner) : _inner(std::move(inner)) {}

		bool Match(RE::InventoryEntryData *entry) const override {
			return !_inner->Match(entry);
		}

	private:
		std::unique_ptr<Condition> _inner;
	};

	class AnyOfCondition final : public Condition {
	public:
		void Add(std::unique_ptr<Condition> cond) {
			_conditions.push_back(std::move(cond));
		}

		bool Match(RE::InventoryEntryData *entry) const override {
			for (const auto &cond : _conditions) {
				if (cond->Match(entry))
					return true;
			}
			return false;
		}

	private:
		std::vector<std::unique_ptr<Condition>> _conditions;
	};

	// ========================================================================
	// Object-field conditions (enchantment / poison / tempered)
	// ========================================================================

	class EnchantmentCondition final : public Condition {
	public:
		void AddMatcher(std::unique_ptr<EffectMatcher> matcher) {
			_matchers.push_back(std::move(matcher));
		}

		bool Match(RE::InventoryEntryData *entry) const override;

	private:
		std::vector<std::unique_ptr<EffectMatcher>> _matchers;
	};

	class PoisonCondition final : public Condition {
	public:
		void AddMatcher(std::unique_ptr<EffectMatcher> matcher) {
			_matchers.push_back(std::move(matcher));
		}

		bool Match(RE::InventoryEntryData *entry) const override;

	private:
		std::vector<std::unique_ptr<EffectMatcher>> _matchers;
	};

	class TemperedCondition final : public Condition {
	public:
		TemperedCondition(std::optional<float> min, std::optional<float> max)
				: _min(min), _max(max) {}

		bool Match(RE::InventoryEntryData *entry) const override;

	private:
		std::optional<float> _min, _max;
	};
}
