#ifndef SDL_build_config_h_
#define SDL_build_config_h_
#include <SDL3/SDL_platform_defines.h>
#if defined(SDL_PLATFORM_WIN32)
#include "SDL_build_config_windows.h"
#if defined(__has_include) && !__has_include(<windows.gaming.input.h>)
#undef HAVE_WINDOWS_GAMING_INPUT_H
#undef SDL_JOYSTICK_WGI
#endif
#elif defined(SDL_PLATFORM_MACOS)
#include "SDL_build_config_macos.h"
#elif defined(SDL_PLATFORM_LINUX)
#include "SDL_build_config_spn.h"
#else
#error spn sdl3: unsupported platform
#endif
#endif
