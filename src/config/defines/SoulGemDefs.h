#pragma once

#include "FormTypes.h"

namespace Config {
	inline const EnumMap<RE::SOUL_LEVEL> GemSizeMap = {
		{"None", RE::SOUL_LEVEL::kNone},       {"Petty", RE::SOUL_LEVEL::kPetty},
		{"Lesser", RE::SOUL_LEVEL::kLesser},   {"Common", RE::SOUL_LEVEL::kCommon},
		{"Greater", RE::SOUL_LEVEL::kGreater}, {"Grand", RE::SOUL_LEVEL::kGrand},
	};

	// SoulSize uses the same enum values
	inline const EnumMap<RE::SOUL_LEVEL> &SoulSizeMap = GemSizeMap;

	enum class SoulGemStatus : std::int32_t {
		kEmpty = 0,
		kPartial = 1,
		kFull = 2,
	};

	inline const EnumMap<SoulGemStatus> SoulGemStatusMap = {
		{"Empty", SoulGemStatus::kEmpty},
		{"Partial", SoulGemStatus::kPartial},
		{"Full", SoulGemStatus::kFull},
	};
}
