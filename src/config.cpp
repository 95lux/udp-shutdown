#include "../include/config.h"
#include "../include/SimpleIni.h"
#include <iostream>
#include <string.h>

Config::Config() {
    wchar_t result[MAX_PATH];
    std::wstring path = std::wstring(result, GetModuleFileName(NULL, result, MAX_PATH));
    wchar_t last = path.back();

    while (last != '\\') {
        path = path.substr(0, path.size() - 1);
        last = path.back();
    }
    path = path + L"config.ini";

    CSimpleIniA ini;
    ini.SetUnicode();
    SI_Error rc = ini.LoadFile(path.c_str());
    if (rc < 0) {
        std::wcout << L"[err] config.ini not found at '" << path << L"' Exiting program. " << std::endl;
        exit(-1);
    };
    CSimpleIniA::TNamesDepend keys;
    ini.GetAllKeys("LOGFILES_PATHS", keys);

    this->port = std::stoi(ini.GetValue("config", "port", "default"));
    this->shutdown_cmd = std::string(ini.GetValue("config", "shutdown_command", "default"));
}
