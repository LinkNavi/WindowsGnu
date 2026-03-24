// plugin.cpp
#include "plugin.h"
#include <iostream>

static void on_init() { std::cout << "hello plugin loaded\n"; }
static void on_exit() { std::cout << "hello plugin unloaded\n"; }

static bool on_command(const std::string& input) {
    if (input != "hello") return false;
    std::cout << "Hello from plugin!\n";
    return true;
}

static PluginAPI api = {
    .name    = "hello",
    .version = "1.0.0",
    .on_init = on_init,
    .on_exit = on_exit,
    .on_command = on_command,
};

extern "C" PluginAPI* ordona_plugin() { return &api; }