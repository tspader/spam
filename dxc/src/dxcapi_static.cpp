#ifdef _MSC_VER
// upstream initializes in DllMain, which a dll runs after its static constructors
#pragma init_seg(".CRT$XCV")
#define DllMain DxcStaticDllMain
#endif
#include "dxcapi.cpp"
#include "DXCompiler.cpp"

#ifdef _MSC_VER
static struct DxcStaticInit {
  DxcStaticInit() { DxcStaticDllMain(nullptr, DLL_PROCESS_ATTACH, nullptr); }
  ~DxcStaticInit() { DxcStaticDllMain(nullptr, DLL_PROCESS_DETACH, this); }
} g_DxcStaticInit;
#endif
