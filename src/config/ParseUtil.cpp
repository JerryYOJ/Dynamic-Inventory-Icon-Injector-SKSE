#include "ParseUtil.h"

namespace Config {
	RE::FormID ParseUtil::ParseFormID(const std::string &a_identifier) {
		std::istringstream ss{a_identifier};
		std::string plugin, id;

		std::getline(ss, plugin, '|');
		std::getline(ss, id);

		RE::FormID rawFormID = 0;
		std::istringstream(id) >> std::hex >> rawFormID;

		if (plugin.empty()) {
			logger::warn("Empty plugin name in FormID: {}", a_identifier);
			return 0;
		}

		const auto dataHandler = RE::TESDataHandler::GetSingleton();
		if (!dataHandler)
			return 0;

		const auto *file = dataHandler->LookupModByName(plugin);
		if (!file) {
			logger::warn("Plugin not found: {}", plugin);
			return 0;
		}

		RE::FormID formID = file->compileIndex << 24;
		if (file->IsLight()) {
			formID += file->smallFileCompileIndex << 12;
		}
		formID += rawFormID;

		return formID;
	}

	std::vector<RE::FormID>
	ParseUtil::ParseFormIDArray(const Json::Value &a_value) {
		std::vector<RE::FormID> result;

		if (a_value.isString()) {
			auto id = ParseFormID(a_value.asString());
			if (id != 0)
				result.push_back(id);
		} else if (a_value.isArray()) {
			for (const auto &elem : a_value) {
				if (elem.isString()) {
					auto id = ParseFormID(elem.asString());
					if (id != 0)
						result.push_back(id);
				}
			}
		}

		return result;
	}
}
