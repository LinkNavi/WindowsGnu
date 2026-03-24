#include "plugin.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

namespace fs = std::filesystem;

// ── helpers ──────────────────────────────────────────────────

static std::vector<std::string> split(const std::string& s)
{
    std::vector<std::string> args;
    std::istringstream ss(s);
    std::string tok;
    while (ss >> tok) args.push_back(tok);
    return args;
}

// ── commands ─────────────────────────────────────────────────

static void cmd_ls(const std::vector<std::string>& args)
{
    fs::path dir = args.size() > 1 ? args[1] : ".";
    try {
        for (const auto& entry : fs::directory_iterator(dir))
        {
            std::string name = entry.path().filename().string();
            if (entry.is_directory()) std::cout << "\x1b[34m" << name << "\x1b[0m  ";
            else                      std::cout << name << "  ";
        }
        std::cout << "\n";
    } catch (...) { std::cerr << "ls: cannot access " << dir << "\n"; }
}

static void cmd_cat(const std::vector<std::string>& args)
{
    if (args.size() < 2) { std::cerr << "cat: missing operand\n"; return; }
    for (size_t i = 1; i < args.size(); i++)
    {
        std::ifstream f(args[i]);
        if (!f.is_open()) { std::cerr << "cat: " << args[i] << ": No such file or directory\n"; continue; }
        std::cout << f.rdbuf();
    }
}

static void cmd_echo(const std::vector<std::string>& args)
{
    for (size_t i = 1; i < args.size(); i++) {
        if (i > 1) std::cout << " ";
        std::cout << args[i];
    }
    std::cout << "\n";
}

static void cmd_mkdir(const std::vector<std::string>& args)
{
    if (args.size() < 2) { std::cerr << "mkdir: missing operand\n"; return; }
    for (size_t i = 1; i < args.size(); i++)
    {
        try { fs::create_directories(args[i]); }
        catch (...) { std::cerr << "mkdir: cannot create directory '" << args[i] << "'\n"; }
    }
}

static void cmd_rm(const std::vector<std::string>& args)
{
    if (args.size() < 2) { std::cerr << "rm: missing operand\n"; return; }
    bool recursive = false;
    for (size_t i = 1; i < args.size(); i++)
    {
        if (args[i] == "-r" || args[i] == "-rf") { recursive = true; continue; }
        try {
            if (recursive) fs::remove_all(args[i]);
            else           fs::remove(args[i]);
        } catch (...) { std::cerr << "rm: cannot remove '" << args[i] << "'\n"; }
    }
}

static void cmd_mv(const std::vector<std::string>& args)
{
    if (args.size() < 3) { std::cerr << "mv: missing operand\n"; return; }
    try { fs::rename(args[1], args[2]); }
    catch (...) { std::cerr << "mv: cannot move '" << args[1] << "' to '" << args[2] << "'\n"; }
}

static void cmd_cp(const std::vector<std::string>& args)
{
    if (args.size() < 3) { std::cerr << "cp: missing operand\n"; return; }
    bool recursive = false;
    std::vector<std::string> paths;
    for (size_t i = 1; i < args.size(); i++)
    {
        if (args[i] == "-r") { recursive = true; continue; }
        paths.push_back(args[i]);
    }
    if (paths.size() < 2) { std::cerr << "cp: missing destination\n"; return; }
    try {
        auto opts = fs::copy_options::overwrite_existing;
        if (recursive) opts |= fs::copy_options::recursive;
        fs::copy(paths[0], paths[1], opts);
    } catch (...) { std::cerr << "cp: cannot copy '" << paths[0] << "' to '" << paths[1] << "'\n"; }
}

static void cmd_pwd(const std::vector<std::string>&)
{
    try { std::cout << fs::current_path().string() << "\n"; }
    catch (...) { std::cerr << "pwd: error\n"; }
}

static void cmd_touch(const std::vector<std::string>& args)
{
    if (args.size() < 2) { std::cerr << "touch: missing operand\n"; return; }
    for (size_t i = 1; i < args.size(); i++)
    {
        std::ofstream f(args[i], std::ios::app);
        if (!f.is_open()) std::cerr << "touch: cannot touch '" << args[i] << "'\n";
    }
}

static void cmd_clear(const std::vector<std::string>&)
{
    std::cout << "\x1b[2J\x1b[H";
}

static void cmd_head(const std::vector<std::string>& args)
{
    if (args.size() < 2) { std::cerr << "head: missing operand\n"; return; }
    int lines = 10;
    std::string file;
    for (size_t i = 1; i < args.size(); i++)
    {
        if (args[i] == "-n" && i + 1 < args.size()) { lines = std::stoi(args[++i]); continue; }
        file = args[i];
    }
    std::ifstream f(file);
    if (!f.is_open()) { std::cerr << "head: " << file << ": No such file or directory\n"; return; }
    std::string line;
    for (int i = 0; i < lines && std::getline(f, line); i++)
        std::cout << line << "\n";
}

static void cmd_tail(const std::vector<std::string>& args)
{
    if (args.size() < 2) { std::cerr << "tail: missing operand\n"; return; }
    int lines = 10;
    std::string file;
    for (size_t i = 1; i < args.size(); i++)
    {
        if (args[i] == "-n" && i + 1 < args.size()) { lines = std::stoi(args[++i]); continue; }
        file = args[i];
    }
    std::ifstream f(file);
    if (!f.is_open()) { std::cerr << "tail: " << file << ": No such file or directory\n"; return; }
    std::vector<std::string> buf;
    std::string line;
    while (std::getline(f, line)) {
        buf.push_back(line);
        if ((int)buf.size() > lines) buf.erase(buf.begin());
    }
    for (const auto& l : buf) std::cout << l << "\n";
}

static void cmd_wc(const std::vector<std::string>& args)
{
    if (args.size() < 2) { std::cerr << "wc: missing operand\n"; return; }
    std::ifstream f(args[1]);
    if (!f.is_open()) { std::cerr << "wc: " << args[1] << ": No such file or directory\n"; return; }
    int lines = 0, words = 0, chars = 0;
    std::string line;
    while (std::getline(f, line)) {
        lines++;
        chars += line.size() + 1;
        std::istringstream ss(line);
        std::string w;
        while (ss >> w) words++;
    }
    std::cout << lines << " " << words << " " << chars << " " << args[1] << "\n";
}

// ── dispatch ─────────────────────────────────────────────────

static bool on_command(const std::string& input)
{
    auto args = split(input);
    if (args.empty()) return false;

    const std::string& cmd = args[0];
    if      (cmd == "ls")    { cmd_ls(args);    return true; }
    else if (cmd == "cat")   { cmd_cat(args);   return true; }
    else if (cmd == "echo")  { cmd_echo(args);  return true; }
    else if (cmd == "mkdir") { cmd_mkdir(args); return true; }
    else if (cmd == "rm")    { cmd_rm(args);    return true; }
    else if (cmd == "mv")    { cmd_mv(args);    return true; }
    else if (cmd == "cp")    { cmd_cp(args);    return true; }
    else if (cmd == "pwd")   { cmd_pwd(args);   return true; }
    else if (cmd == "touch") { cmd_touch(args); return true; }
    else if (cmd == "clear") { cmd_clear(args); return true; }
    else if (cmd == "head")  { cmd_head(args);  return true; }
    else if (cmd == "tail")  { cmd_tail(args);  return true; }
    else if (cmd == "wc")    { cmd_wc(args);    return true; }
    return false;
}

// ── plugin entry ─────────────────────────────────────────────

static const char* cmds[] = {
    "ls","cat","echo","mkdir","rm","mv","cp",
    "pwd","touch","clear","head","tail","wc",
    nullptr
};

static PluginAPI api = {
    .name       = "WindowsGnu",
    .version    = "1.0.0",
    .commands   = cmds,
    .on_command = on_command,
};

extern "C" PluginAPI* ordona_plugin() { return &api; }