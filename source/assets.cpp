#if defined(PLATFORM_WIN32)
#include "OS/win32/assets.cpp"
#elif defined(PLATFORM_WEB)
#include "OS/web/assets.cpp"
#endif
