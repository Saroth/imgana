#include <stdio.h>

#include "version.h"

static char version_string[32];

unsigned int imgana_get_version(void)
{
    return IMGANA_VERSION;
}

const char *imgana_get_version_str(void)
{
    snprintf(version_string, sizeof(version_string), "%d.%d.%d",
            (IMGANA_VERSION >> 16) & 0xFF,
            (IMGANA_VERSION >>  8) & 0xFF,
            (IMGANA_VERSION >>  0) & 0xFF);

    return version_string;
}

