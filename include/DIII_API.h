// DIII_API.h — other plugins #include this
#pragma once

#include <cstdint>
#include <functional>
#include <json/json.h>
#include <RE/Skyrim.h>

namespace DIII {
    // Base class other plugins inherit from to create custom conditions
    class ICondition {
    public:
        virtual ~ICondition() = default;
        virtual bool Match(RE::InventoryEntryData* entry) const = 0;
    };

    // Builder signature: takes JSON value, returns a condition (or nullptr)
    using ConditionBuilder = std::function<
        std::unique_ptr<ICondition>(const Json::Value& value)
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

    // Convenience: call from your plugin's MessagingInterface callback
    // Returns nullptr if DIII isn't installed
    inline IAPI* RequestAPI(const SKSE::MessagingInterface* messaging) {
        IAPI* api = nullptr;
        messaging->Dispatch(
            kMessage_GetAPI,
            &api,
            sizeof(IAPI*),
            "DynamicInventoryIconInjector"
        );
        return api;
    }
}
