#include <stdio.h>

#include "libanalyzer.h"

/**
 * version format:
 *      0x00AAIIPP
 *      AA: major version
 *      II: minor version
 *      PP: patch
 */
#define ANALYZER_VERSION    0x00000000

static char version_string[32];

unsigned int analyzer_version(void)
{
    return ANALYZER_VERSION;
}

const char *analyzer_version_str(void)
{
    snprintf(version_string, sizeof(version_string), "%d.%d.%d",
            (ANALYZER_VERSION >> 16) & 0xFF,
            (ANALYZER_VERSION >>  8) & 0xFF,
            (ANALYZER_VERSION >>  0) & 0xFF);

    return version_string;
}

