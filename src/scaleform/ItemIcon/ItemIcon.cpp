#include "ItemIcon.h"

#include "../../config/ConfigManager.h"
#include "../../swfhelper/ImportData.h"

//this global is required to make string_view work dont remove
std::unordered_map<std::string, std::unordered_set<std::string>> registeredIcons;

std::vector<std::string> registeredExports;
std::vector<ImportData::loadReq> loadRequest;

static void InventoryScaleform(RE::GFxMovieView* a_view, RE::GFxValue* a_object, RE::InventoryEntryData* a_item) {
	
	auto&& icons = Config::ConfigManager::getInstance()->GetIcons(a_item);
	if (icons.empty()) return;

	RE::GFxValue iconArr;
	a_view->CreateArray(&iconArr);
	iconArr.SetArraySize(static_cast<std::uint32_t>(icons.size()));
	for (std::uint32_t idx = 0; idx < icons.size(); idx++) {
		RE::GFxValue iconData;
		a_view->CreateObject(&iconData);

		iconData.SetMember("label", icons[idx]->label.data());
		if(!icons[idx]->replace.empty()) iconData.SetMember("replace", icons[idx]->replace.data());

		iconArr.SetElement(idx, iconData);
	}
	a_object->SetMember("_DIIIIcons", iconArr);

	return;
}

void ItemIcon::Install()
{
	REL::Relocation<std::uintptr_t> Inv{ RE::VTABLE_InventoryMenu[0] };
	_PostCreateInv = Inv.write_vfunc(0x2, &PostCreateInv);

	REL::Relocation<std::uintptr_t> Cont{ RE::VTABLE_ContainerMenu[0] };
	_PostCreateCont = Cont.write_vfunc(0x2, &PostCreateCont);

	REL::Relocation<std::uintptr_t> Bart{ RE::VTABLE_BarterMenu[0] };
	_PostCreateBart = Bart.write_vfunc(0x2, &PostCreateBart);

	REL::Relocation<std::uintptr_t> Gift{ RE::VTABLE_GiftMenu[0] };
	_PostCreateGift = Gift.write_vfunc(0x2, &PostCreateGift);

	SKSE::GetScaleformInterface()->Register(InventoryScaleform);
}

void ItemIcon::InstallLate()
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

			//Calculate icon positions and reset vanilla icon
			auto iconY = entryHeight.GetNumber() * 0.25;

			RE::GFxValue textField, textWidth, textX;

			a_entryField.GetMember("_width", &textWidth);
			a_entryField.GetMember("_x", &textX);

			auto iconPos = textX.GetNumber() + textWidth.GetNumber() + 5.0;
			auto iconSpace = entryHeight.GetNumber() * 0.625;

			
			static constexpr std::array vanillaIcons{
				"bestIcon", "favoriteIcon", "poisonIcon",
				"stolenIcon", "enchIcon", "readIcon"
			};
			for (auto&& name : vanillaIcons) {
				RE::GFxValue icon;
				a_params.thisPtr->GetMember(name, &icon);

				if (icon.IsUndefined()) {
					logger::critical("Failed to find vanilla icon {}???", name);
					continue;
				}

				icon.SetMember("_visible", true);

				RE::GFxValue currentFrame;
				icon.GetMember("_currentframe", &currentFrame);

				if (currentFrame.IsNumber() && currentFrame.GetNumber() == 2) {
					iconPos += iconSpace;
				}
			}

			//Show matched icons
			RE::GFxValue iconArr;
			a_entryObject.GetMember("_DIIIIcons", &iconArr);

			if (iconArr.IsUndefined()) return;

			for (std::uint32_t idx = 0, append_idx = 0, size = iconArr.GetArraySize(); idx < size; idx++) {
				RE::GFxValue iconData;
				iconArr.GetElement(idx, &iconData);

				RE::GFxValue iconName;
				iconData.GetMember("label", &iconName);
				RE::GFxValue replace;
				iconData.GetMember("replace", &replace);

				RE::GFxValue Icon;
				a_params.thisPtr->GetMember(iconName.GetString(), &Icon);
				if (Icon.IsUndefined()) {
					a_params.thisPtr->AttachMovie(&Icon, iconName.GetString(), iconName.GetString());

					if (Icon.IsUndefined()) {
						logger::error("Failed to attach icon {}", iconName.GetString());
						continue;
					}

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

				if (replace.IsUndefined()) {
					//Set visibility
					{
						Icon.SetMember("_visible", true);
					}

					//Set icon position
					{
						auto iconX = iconPos + (iconSpace * append_idx);

						Icon.SetMember("_y", iconY);
						Icon.SetMember("_x", iconX);
					}

					append_idx++;
				}
				else {
					RE::GFxValue replacedIcon;
					a_params.thisPtr->GetMember(replace.GetString(), &replacedIcon);

					if (replacedIcon.IsUndefined()) {
						logger::error("Failed to find replace icon {}, make sure the label is correct and the icon is attached to the movie", replace.GetString());
						continue;
					}

					//Set visibility
					{
						replacedIcon.SetMember("_visible", false);
						Icon.SetMember("_visible", true);
					}

					//Set icon position
					{
						RE::GFxValue replaceY, replaceX;
						replacedIcon.GetMember("_y", &replaceY);
						replacedIcon.GetMember("_x", &replaceX);

						Icon.SetMember("_y", replaceY);
						Icon.SetMember("_x", replaceX);
					}
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

void ItemIcon::PostCreateInv(RE::IMenu* thiz)
{
	Setup(thiz);
	return _PostCreateInv(thiz);
}

void ItemIcon::PostCreateCont(RE::IMenu* thiz)
{
	Setup(thiz);
	return _PostCreateCont(thiz);
}

void ItemIcon::PostCreateBart(RE::IMenu* thiz)
{
	Setup(thiz);
	return _PostCreateBart(thiz);
}

void ItemIcon::PostCreateGift(RE::IMenu* thiz)
{
	Setup(thiz);
	return _PostCreateGift(thiz);
}
