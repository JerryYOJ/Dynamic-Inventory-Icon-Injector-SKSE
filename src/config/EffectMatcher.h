#pragma once

namespace Config {
	// Base class — matches against an individual magic effect

	class EffectMatcher {
	public:
		virtual ~EffectMatcher() = default;
		[[nodiscard]] virtual bool Match(RE::Effect *effect) const = 0;
	};

	// ========================================================================
	// Effect-level matchers
	// ========================================================================

	template <typename T> class EffectExactMatch final : public EffectMatcher {
	public:
		using Getter = T (*)(RE::Effect *);

		EffectExactMatch(T expected, Getter getter)
			: _expected(expected), _getter(getter) {}

		bool Match(RE::Effect *effect) const override {
			return _getter(effect) == _expected;
		}

	private:
		T _expected;
		Getter _getter;
	};

	class EffectRangeMatch final : public EffectMatcher {
	public:
		using Getter = float (*)(RE::Effect *);

		EffectRangeMatch(std::optional<float> min, std::optional<float> max,
						Getter getter)
			: _min(min), _max(max), _getter(getter) {}

		bool Match(RE::Effect *effect) const override {
			float value = _getter(effect);
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

	class EffectFormIdMatch final : public EffectMatcher {
	public:
		EffectFormIdMatch(std::vector<RE::FormID> ids) : _ids(std::move(ids)) {}

		bool Match(RE::Effect *effect) const override {
			if (!effect || !effect->baseEffect)
				return false;
			return std::find(_ids.begin(), _ids.end(), effect->baseEffect->formID) !=
				_ids.end();
		}

	private:
		std::vector<RE::FormID> _ids;
	};

	class EffectBitfieldMatch final : public EffectMatcher {
	public:
		EffectBitfieldMatch(std::uint32_t flags) : _flags(flags) {}

		bool Match(RE::Effect *effect) const override {
			if (!effect || !effect->baseEffect)
				return false;
			auto effectFlags =
				static_cast<std::uint32_t>(effect->baseEffect->data.flags.underlying());
			return (effectFlags & _flags) != 0;
		}

	private:
		std::uint32_t _flags;
	};

	class EffectKeywordMatch final : public EffectMatcher {
	public:
		EffectKeywordMatch(RE::BGSKeyword *keyword) : _keyword(keyword) {}

		bool Match(RE::Effect *effect) const override {
			if (!effect || !effect->baseEffect || !_keyword)
				return false;
			return effect->baseEffect->HasKeyword(_keyword);
		}

	private:
		RE::BGSKeyword *_keyword;
	};
}
