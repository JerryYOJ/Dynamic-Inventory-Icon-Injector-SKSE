#pragma once

namespace Config {
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
