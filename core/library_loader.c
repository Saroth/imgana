#include <stdio.h>
#include <dlfcn.h>

#include "libanalyzer.h"
#include "library_loader.h"

static const char *func_name_list[] = {
    "analyzer_init",
    "analyzer_free",

    "analyzer_set_memory_alloc",
    "analyzer_set_debug",
    "analyzer_set_mark_point",
    "analyzer_set_mark_line",

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

libana_function_list ana_func_list[LIBANA_FUNC_MAX];
static void *libana_handler = 0;

int libana_init(void)
{
    
}

const char *libana_version_str(void)
{
    return 0;
}

