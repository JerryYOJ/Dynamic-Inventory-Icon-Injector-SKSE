#pragma once

namespace Config {
	// Case-insensitive string comparator for enum maps
	struct CaseInsensitiveCompare {
		bool operator()(const std::string &a, const std::string &b) const {
			return std::lexicographical_compare(
					a.begin(), a.end(), b.begin(), b.end(),
					[](char ca, char cb) { return std::tolower(ca) < std::tolower(cb); });
		}
	};

	template <typename T>
	using EnumMap = std::map<std::string, T, CaseInsensitiveCompare>;

	inline const EnumMap<RE::FormType> FormTypeMap = {
		{"Ammo", RE::FormType::Ammo},     {"Armor", RE::FormType::Armor},
		{"Book", RE::FormType::Book},     {"Ingredient", RE::FormType::Ingredient},
		{"Key", RE::FormType::KeyMaster},
		{"MiscItem", RE::FormType::Misc}, {"Potion", RE::FormType::AlchemyItem},
		{"Scroll", RE::FormType::Scroll}, {"SoulGem", RE::FormType::SoulGem},
		{"Weapon", RE::FormType::Weapon},
	};

	template <typename T>
	bool TryGetEnum(const EnumMap<T> &map, const std::string &key, T &outValue) {
		if (auto it = map.find(key); it != map.end()) {
			outValue = it->second;
			return true;
		}
		return false;
	}
}
