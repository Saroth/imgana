#include <stdio.h>
#include <dlfcn.h>

#include "library_loader.h"

static const char *func_name_list[] = {
    "analyzer_init",
    "analyzer_free",

    "analyzer_set_memory_alloc",
    "analyzer_set_debug",
    "analyzer_set_mark_point",
    "analyzer_set_mark_line",

    "analyzer_import_bitmap",
    "analyzer_import_bmp",
    "analyzer_export_bitmap",
    "analyzer_export_data",

    "analyzer_start",
    "analyzer_is_running",
    "analyzer_stop",

    "analyzer_version",
    "analyzer_version_str",

    0,
};

static void *libana_handler = 0;
static void *ana_func_list[LIBANA_FUNC_MAX];
libana_functions *libana = (libana_functions *)ana_func_list;

int libana_init(void)
{
    return 0;
}

const char *libana_version_str(void)
{
    return 0;
}

