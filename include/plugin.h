// include/plugin.h
#pragma once
#include <string>

struct PluginAPI {
    const char* name     = nullptr;
    const char* version  = nullptr;
    void (*on_init)()                                = nullptr;
    void (*on_exit)()                                = nullptr;
    bool (*on_command)(const std::string& input)     = nullptr;
    std::string (*on_hint)(const std::string& input) = nullptr;
    void (*on_cd)(const std::string& newdir)         = nullptr;
};

extern "C" PluginAPI* ordona_plugin();