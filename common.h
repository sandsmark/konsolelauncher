#pragma once
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

enum COMMAND {
    COMMAND_QUIT,
    COMMAND_LAUNCH
};

typedef struct Command {
    int commandId;
    int argCount;
} Command;

char *generatePath(const char *suffix)
{
    char *runtimeDir = getenv("XDG_RUNTIME_DIR");
    char *seat = getenv("XDG_SEAT");

    assert(runtimeDir);
    assert(seat);

    const char *formatString = "%s/konsole_launcher_%s%s";

    // A bit longer than necessary because of the %, but better with too much
    const size_t stringLength = strlen(formatString) + strlen(runtimeDir) + strlen(seat) + strlen(suffix);

    // Plus one because I want to path[stringLength]
    char *path = (char*)calloc(stringLength + 1, sizeof(char));
    assert(path);
    if (!path) {
        puts("Failed to allocate path variable");
        return NULL;
    }

    snprintf(path, stringLength, formatString, runtimeDir, seat, suffix);
    path[stringLength] = 0;

    return path;
}

