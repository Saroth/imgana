#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <dlfcn.h>

#include <unistd.h>

#include "library_loader.h"

static const char *library_file = "libanalyzer_demo.so";
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


LibraryLoader::LibraryLoader()
{
    flag_running = false;
    handler = 0;
    f_debug = 0;
    memset(func_list, 0, sizeof(func_list));
    funcs = (const libana_functions *)func_list;
}

int LibraryLoader::sdb_out_info(const char *file, size_t line,
        const char *fmt, ...)
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

    return printf("%20s:%04ld  %s\n", file, line, buf);
}



void LibraryLoader::set_debug(
        int (*f)(void *, const char *, size_t, const char *), void *p)
{
    f_debug = f;
    p_debug = p;
}

int LibraryLoader::load()
{
    unload();
    sdb_out_info(__FILE__, __LINE__, "loading library: %s.", library_file);
    void *h;
    if ((h = dlopen(library_file, RTLD_LAZY)) == 0) {
        sdb_out_info(__FILE__, __LINE__,
                "library load failed: %s.", dlerror());
        return LIBANA_ERR_LOAD_LIBRARY_FAILED;
    }

    unsigned int i = 0;
    for (; i < LIBANA_FUNC_MAX; i++) {
        func_list[i] = dlsym(h, func_name_list[i]);
        char *err = dlerror();
        if (err) {
            sdb_out_info(__FILE__, __LINE__,
                    "dlsym(%s) error: %s", func_name_list[i], err);
            dlclose(h);
            return LIBANA_ERR_SYMBOL_IS_NOT_FOUND;
        }
    }
    handler = h;

    sdb_out_info(__FILE__, __LINE__, "library version: %s", library_version());
    sdb_out_info(__FILE__, __LINE__, "load successfully.");
    return 0;
}

void LibraryLoader::unload()
{
    if (handler) {
        sdb_out_info(__FILE__, __LINE__, "close library.");
        memset(func_list, 0, sizeof(func_list));
        dlclose(handler);
        handler = 0;
    }
}

bool LibraryLoader::is_loaded()
{
    return handler ? 1 : 0;
}

const char *LibraryLoader::library_version(void)
{
    if (is_loaded()) {
        return funcs->version_str();
    }
    return "";
}



int LibraryLoader::run(int type)
{
    if (!is_loaded()) {
        sdb_out_info(__FILE__, __LINE__, "library not loaded.");
        return LIBANA_ERR_LIBRARY_NOT_LOADED;
    }
    flag_running = true;
    funcs->init(&context);
    funcs->set_memory_alloc(&context, 0, 0, 0);
    funcs->set_debug(&context, f_debug, p_debug);
#warning "TODO: mark callback"
    funcs->set_mark_point(&context, 0, 0);
    funcs->set_mark_line(&context, 0, 0);
#warning "TODO: import image"
    funcs->import_bmp(&context, 0, 0);
    int ret = funcs->start(&context, type);
    if (ret) {
        sdb_out_info(__FILE__, __LINE__, "analyze failed, return:%x.", ret);
    }
    funcs->free(&context);
    flag_running = false;

    return 0;
}

bool LibraryLoader::is_running()
{
    return flag_running;
}

void LibraryLoader::stop()
{
    if (!is_running()) {
        sdb_out_info(__FILE__, __LINE__, "analyzer is not running.");
        return;
    }
    sdb_out_info(__FILE__, __LINE__, "stop analyzer.");
    funcs->stop(&context);
}

bool LibraryLoader::is_busy()
{
    if (!is_loaded()) {
        sdb_out_info(__FILE__, __LINE__, "library not loaded.");
        return false;
    }
    return funcs->is_running(&context) ? true : false;
}


