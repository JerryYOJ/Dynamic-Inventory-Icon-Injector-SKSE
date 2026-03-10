// DIII_API.h — other plugins #include this
#pragma once

#include <cstdint>
#include <functional>
#include <json/json.h>

namespace DIII {
    // Base class other plugins inherit from to create custom conditions
    class ICondition {
    public:
        virtual ~ICondition() = default;
        virtual bool Match(RE::InventoryEntryData* entry) const = 0;
    };

    // Builder signature: takes JSON value, returns a condition (or nullptr)
    using ConditionBuilder = std::function<
        std::unique_ptr<ICondition>(const Json::Value& value, RE::FormType type)
    >;

    // The API interface — stable ABI via pure virtual
    class IAPI {
    public:
        virtual ~IAPI() = default;

        // Register a custom match field builder
        // name = JSON key (e.g. "myModCondition")
        // builder = factory that creates the condition from JSON
        virtual bool RegisterCondition(
            const char* name,
            ConditionBuilder builder
        ) = 0;

        // API version for compatibility checks
        virtual uint32_t GetVersion() const = 0;
    };

    // Message type for SKSE messaging
    constexpr uint32_t kMessage_GetAPI = 0xD111;

	//Example of how a plugin would register a condition builder
    //SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    //    SKSE::Init(skse);
    //    DIII::ListenForRegistration([](SKSE::MessagingInterface::Message* msg) {
    //        if (msg->type == DIII::kMessage_RegisterConditions) {
    //            auto* api = static_cast<DIII::IAPI*>(msg->data);
    //            api->RegisterCondition("myCustomField", [](const Json::Value& match, RE::FormType type)
    //                -> std::unique_ptr<DIII::ICondition> {
    //                    // build and return your condition
    //                });
    //        }
    //        });
    //    return true;
    //}
    inline void ListenForRegistration(SKSE::MessagingInterface::EventCallback* cb) {
		SKSE::GetMessagingInterface()->RegisterListener("DynamicInventoryIconInjector", cb);
    }
}
