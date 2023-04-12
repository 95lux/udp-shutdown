#include "../include/SimpleIni.h"
#include <stdio.h>
#include <string.h>

class Config {
public:
    int port;
    const char *shutdown_cmd;
    Config();
};
