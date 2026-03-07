#include <MinHook.h>

namespace Hooks {
    void Install() { 
        MH_Initialize();

        

        MH_EnableHook(MH_ALL_HOOKS);
        return;
    }
}