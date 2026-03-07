#pragma once

#include "FormTypes.h"

namespace Config {
	enum class PotionType : std::int32_t {
		kFood = 0,
		kPoison = 1,
		kMedicine = 2
	};

	inline const EnumMap<PotionType> PotionTypeMap = {
			{"Food", PotionType::kFood},
			{"Poison", PotionType::kPoison},
			{"Medicine", PotionType::kMedicine}
	};
}
