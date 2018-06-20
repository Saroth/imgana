#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <dlfcn.h>

#include <unistd.h>

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
static void *func_list[LIBANA_FUNC_MAX];
const libana_functions *libana = (const libana_functions *)func_list;
static const char *library_file = "libanalyzer_demo.so";
static func_analyzer_bio_debug f_debug;
static void *p_debug;

int sdb_out_info(const char *file, size_t line, const char *fmt, ...)
{
    char buf[1024];
    va_list va;
    va_start(va, fmt);
    vsnprintf(buf, sizeof(buf), fmt, va);
    va_end(va);

    file = strrchr(file, '\\') ? (strrchr(file, '\\') + 1) :
        strrchr(file, '/') ? (strrchr(file, '/') + 1) : file;
    if (f_debug) {
        return f_debug(p_debug, file, line, buf);
    }
    printf("%20s:%04ld  %s", file, line, buf);
    return 0;
}



void libana_set_debug(func_analyzer_bio_debug f, void *p)
{
    f_debug = f;
    p_debug = p;
}

void libana_unload(void)
{
    sdb_out_info(__FILE__, __LINE__, "close library.");
    if (libana_handler) {
        memset(func_list, 0, sizeof(func_list));
        dlclose(libana_handler);
    }
}

int libana_load(void)
{
    libana_unload();
    sdb_out_info(__FILE__, __LINE__, "loading library: %s.", library_file);
    if ((libana_handler = dlopen(library_file, RTLD_LAZY)) == 0) {
        sdb_out_info(__FILE__, __LINE__,
                "library load failed: %s.", dlerror());
        return LIBANA_ERR_LOAD_LIBRARY_FAILED;
    }

    unsigned int i = 0;
    for (; i < LIBANA_FUNC_MAX; i++) {
        func_list[i] = dlsym(libana_handler, func_name_list[i]);
        char *err = dlerror();
        if (err) {
            sdb_out_info(__FILE__, __LINE__,
                    "dlsym(%s) error: %s", func_name_list[i], err);
            dlclose(libana_handler);
            return LIBANA_ERR_SYMBOL_IS_NOT_FOUND;
        }
    }

    sdb_out_info(__FILE__, __LINE__, "load finish.");
    return 0;
}

int libana_is_available(void)
{
    return libana_handler ? 1 : 0;
}

