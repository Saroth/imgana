#include <stdio.h>

#include "version.h"

/**
 * version format:
 *      0x00AAIIPP
 *      AA: major version
 *      II: minor version
 *      PP: patch
 */
#define IMGANA_VERSION  0x00000100

static char version_string[32];

unsigned int imgana_version(void)
{
    return IMGANA_VERSION;
}

const char *imgana_version_str(void)
{
    snprintf(version_string, sizeof(version_string), "%d.%d.%d",
            (IMGANA_VERSION >> 16) & 0xFF,
            (IMGANA_VERSION >>  8) & 0xFF,
            (IMGANA_VERSION >>  0) & 0xFF);

    return version_string;
}

