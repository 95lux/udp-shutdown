#include "../include/SimpleIni.h"
#include <stdio.h>
#include <string.h>

class Config {
public:
    int port;
    std::string shutdown_cmd;
    Config();
};
