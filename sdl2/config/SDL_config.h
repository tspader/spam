#ifndef SDL_config_h_
#define SDL_config_h_
#include "SDL_platform.h"
#if defined(__WIN32__)
#include "SDL_config_windows.h"
#undef HAVE_WINDOWS_GAMING_INPUT_H
#undef SDL_JOYSTICK_WGI
#elif defined(__MACOSX__)
#include "SDL_config_macosx.h"
#elif defined(__LINUX__)
#include "SDL_config_spn.h"
#else
#error spn sdl2: unsupported platform
#endif
#endif
