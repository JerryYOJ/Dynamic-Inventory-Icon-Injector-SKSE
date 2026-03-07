#include "QuestItemIcon.h"

#include "../../config/ConfigManager.h"
#include "../../swfhelper/ImportData.h"

std::unordered_map<std::string, std::unordered_set<std::string>> registeredIcons;
std::vector<std::string> registeredExports;
std::vector<ImportData::loadReq> loadRequest;

static void InventoryScaleform(RE::GFxMovieView* a_view, RE::GFxValue* a_object, RE::InventoryEntryData* a_item) {
	
	auto&& icons = Config::ConfigManager::getInstance()->GetIcons(a_item);
	if (icons.empty()) return;

	RE::GFxValue iconArr;
	a_view->CreateArray(&iconArr);
	iconArr.SetArraySize(static_cast<std::uint32_t>(icons.size()));
	for (std::uint32_t idx = 0; idx < icons.size(); idx++) iconArr.SetElement(idx, icons[idx]->label.data());
	a_object->SetMember("_DIIIIcons", iconArr);

	return;
}

void QuestItemIcon::Install()
{
	REL::Relocation<std::uintptr_t> Inv{ RE::VTABLE_InventoryMenu[0] };
	_PostCreateInv = Inv.write_vfunc(0x2, &PostCreateInv);

	REL::Relocation<std::uintptr_t> Cont{ RE::VTABLE_ContainerMenu[0] };
	_PostCreateCont = Cont.write_vfunc(0x2, &PostCreateCont);

	SKSE::GetScaleformInterface()->Register(InventoryScaleform);
}

void QuestItemIcon::InstallLate()
{
	Config::ConfigManager::getInstance()->LoadConfigs();

	registeredIcons = Config::ConfigManager::getInstance()->GetLoadedIcons();

	for (auto& [source, labelSet] : registeredIcons) {
		registeredExports.insert(registeredExports.end(), labelSet.begin(), labelSet.end());
		loadRequest.push_back({ .sourcePath = source,
						 .exports = {labelSet.begin(), labelSet.end()},
						 .resources = {} });
	}
	return;
}

class formatNameHook : public RE::GFxFunctionHandler {
public:
	formatNameHook(const RE::GFxValue&& old) : oldFunc(old) {}

	void Call(Params& a_params) override {
		oldFunc.Invoke("call", a_params.retVal, a_params.argsWithThisRef, a_params.argCount + 1);

		if (a_params.argCount != 3) {
			logger::debug("Expected 3 arguments, received {}", a_params.argCount);
			return;
		}

		auto& a_entryField = a_params.args[0];
		auto& a_entryObject = a_params.args[1];
		auto& a_state = a_params.args[2];

		if (a_state.IsObject() && a_entryObject.IsObject() && a_entryField.IsObject()) {

			RE::GFxValue list, layout, entryHeight;
			a_state.GetMember("list", &list);
			list.GetMember("layout", &layout);
			layout.GetMember("entryHeight", &entryHeight);

			RE::GFxValue name;
			a_entryObject.GetMember("text", &name);

			//Hide previous icons
			for (auto&& it : registeredExports) {
				RE::GFxValue Icon;
				a_params.thisPtr->GetMember(it.data(), &Icon);
				if (Icon.IsUndefined()) continue;
				Icon.SetMember("_visible", false);
			}

			//Show matched icons
			RE::GFxValue iconArr;
			a_entryObject.GetMember("_DIIIIcons", &iconArr);

			if (iconArr.IsUndefined()) return;

			for (std::uint32_t idx = 0, size = iconArr.GetArraySize(); idx < size; idx++) {
				RE::GFxValue iconName;
				iconArr.GetElement(idx, &iconName);

				RE::GFxValue Icon;
				a_params.thisPtr->GetMember(iconName.GetString(), &Icon);
				if (Icon.IsUndefined()) {
					a_params.thisPtr->AttachMovie(&Icon, iconName.GetString(), iconName.GetString());

					//Set height width only needed once
					{
						RE::GFxValue nativeHeight, nativeWidth;
						Icon.GetMember("_height", &nativeHeight);
						Icon.GetMember("_width", &nativeWidth);
						auto height = entryHeight.GetNumber() * 0.5;
						auto width = height * (nativeWidth.GetNumber() / nativeHeight.GetNumber());

						Icon.SetMember("_height", height);
						Icon.SetMember("_width", width);
					}
				}

				//Set visibility
				{
					Icon.SetMember("_visible", true);
				}

				//Set icon position
				{
					auto iconY = entryHeight.GetNumber() * 0.25;

					RE::GFxValue textField, textWidth, textX;

					a_entryField.GetMember("_width", &textWidth);
					a_entryField.GetMember("_x", &textX);

					auto iconPos = textX.GetNumber() + textWidth.GetNumber() + 5.0;
					auto iconSpace = entryHeight.GetNumber() * 0.625;

					//excluded stolen for this cuz its more complicated
					static constexpr std::array flags{ "bestInClass","favorite","isPoisoned","isEnchanted", "isRead" };
					for (auto&& flag : flags) {
						RE::GFxValue f;
						a_entryObject.GetMember(flag, &f);
						if (f.IsBool() && f.GetBool()) iconPos += iconSpace;
						else if (f.IsNumber() && f.GetNumber() == 1) iconPos += iconSpace;
					}
					RE::GFxValue steal, showsteal;
					a_state.GetMember("showStolenIcon", &showsteal);
					if (showsteal.IsBool()) {
						if (a_entryObject.GetMember("isStolen", &steal); steal.IsBool() && steal.GetBool() && showsteal.GetBool()) iconPos += iconSpace;
						else if (a_entryObject.GetMember("isStealing", &steal); steal.IsBool() && steal.GetBool() && showsteal.GetBool()) iconPos += iconSpace;
					}

					iconPos += idx * iconSpace;

					Icon.SetMember("_y", iconY);
					Icon.SetMember("_x", iconPos);
				}
			}
		}
	}
protected:
	RE::GFxValue oldFunc;
};

static void Setup(RE::IMenu* thiz){
	RE::GFxValue proto;
	thiz->uiMovie->GetVariable(&proto, "_global.InventoryListEntry.prototype");

	if (!proto.IsObject()) {
		logger::error("Failed to get InventoryListEntry prototype");
		return;
	}

	logger::debug("Hook list formatName");
	{
		RE::GFxValue oldF;
		proto.GetMember("formatName", &oldF);

		auto Impl = RE::make_gptr<formatNameHook>(std::move(oldF));

		RE::GFxValue newF;
		thiz->uiMovie->CreateFunction(&newF, Impl.get());
		proto.SetMember("formatName", newF);
	}

	logger::debug("Dark magic inject swf resource");
	{
		ImportData::ImportResources(ImportData::GetMovieDefImpl(thiz->uiMovie->GetMovieDef()), loadRequest);

		std::for_each(loadRequest.begin(), loadRequest.end(), [](auto&& req) {
			req.resources.clear();
		});
	}
}

void QuestItemIcon::PostCreateInv(RE::IMenu* thiz)
{
	Setup(thiz);
	return _PostCreateInv(thiz);
}

void QuestItemIcon::PostCreateCont(RE::IMenu* thiz)
{
	Setup(thiz);
	return _PostCreateCont(thiz);
}
