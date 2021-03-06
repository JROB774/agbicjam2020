#ifndef MAIN_WEB_HPP
#define MAIN_WEB_HPP

#define PLATFORM_WEB

#include <emscripten.h>

#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <ctime>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <random>
#include <map>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <GON/GON.hpp>
#include <GON/GON.cpp>

// Header includes.
#include "utility.hpp"
#include "error.hpp"
#include "assets.hpp"
#include "image.hpp"
#include "font.hpp"
#include "settings.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include "timer.hpp"
#include "input.hpp"
#include "mixer.hpp"
#include "fade.hpp"
#include "save.hpp"
#include "animation.hpp"
#include "particle.hpp"
#include "spike.hpp"
#include "bone.hpp"
#include "break.hpp"
#include "crushboi.hpp"
#include "spitboy.hpp"
#include "walkboy.hpp"
#include "chargeboy.hpp"
#include "map.hpp"
#include "dog.hpp"
#include "gui.hpp"
#include "collision.hpp"
#include "world.hpp"
#include "camera.hpp"
#include "badges.hpp"
#include "menu.hpp"
#include "stats.hpp"
#include "game.hpp"
#include "pause.hpp"
#include "application.hpp"

// Source includes.
#include "utility.cpp"
#include "error.cpp"
#include "assets.cpp"
#include "image.cpp"
#include "font.cpp"
#include "settings.cpp"
#include "window.cpp"
#include "renderer.cpp"
#include "timer.cpp"
#include "input.cpp"
#include "mixer.cpp"
#include "fade.cpp"
#include "save.cpp"
#include "animation.cpp"
#include "particle.cpp"
#include "spike.cpp"
#include "break.cpp"
#include "bone.cpp"
#include "crushboi.cpp"
#include "spitboy.cpp"
#include "walkboy.cpp"
#include "chargeboy.cpp"
#include "map.cpp"
#include "dog.cpp"
#include "gui.cpp"
#include "collision.cpp"
#include "world.cpp"
#include "camera.cpp"
#include "badges.cpp"
#include "menu.cpp"
#include "stats.cpp"
#include "game.cpp"
#include "pause.cpp"
#include "application.cpp"

#endif /* MAIN_WEB_HPP */
