#pragma once

#include <json/json.h>

namespace Config {
class ParseUtil {
	public:
		ParseUtil() = delete;

		static RE::FormID ParseFormID(const std::string &a_identifier);

		static std::vector<RE::FormID> ParseFormIDArray(const Json::Value &a_value);
	};
}
