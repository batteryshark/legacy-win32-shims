#include <Windows.h>

/* Empty export used by simple loaders and inspection tools. */
__declspec(dllexport) void dropkick(void) {}

static void loadLocalWinmm(void) {
    HMODULE loadedWinmm = GetModuleHandleA("winmm.dll");
    if (loadedWinmm) {
        FreeLibrary(loadedWinmm);
    }

    LoadLibraryA("./winmm.dll");
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    (void)hModule;
    (void)lpReserved;

    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        loadLocalWinmm();
    }

    return TRUE;
}
