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
    image_data = 0;
    image_data_size = 0;
}

LibraryLoader::~LibraryLoader()
{
    delete []image_data;
    image_data = 0;
    image_data_size = 0;
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



void LibraryLoader::set_debug(func_analyzer_bio_debug f, void *p)
{
    f_debug = f;
    p_debug = p;
}

void LibraryLoader::set_mark_point(func_analyzer_bio_mark_point f, void *p)
{
    f_mark_point = f;
    p_mark_point = p;
}

void LibraryLoader::set_mark_line(func_analyzer_bio_mark_line f, void *p)
{
    f_mark_line = f;
    p_mark_line = p;
}

void LibraryLoader::set_image(const char *file_name)
{
    sdb_out_info(__FILE__, __LINE__, "load image: %s.", file_name);
#warning "TODO: load image"
    image_data_size = 1;
    delete []image_data;
    image_data = new unsigned char[image_data_size];
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
    if (!image_data) {
        sdb_out_info(__FILE__, __LINE__, "image not loaded.");
        return LIBANA_ERR_IMAGE_NOT_LOADED;
    }
    flag_running = true;
    funcs->init(&context);
    funcs->set_memory_alloc(&context, 0, 0, 0);
    funcs->set_debug(&context, f_debug, p_debug);
    funcs->set_mark_point(&context, f_mark_point, p_mark_point);
    funcs->set_mark_line(&context, f_mark_line, p_mark_line);
    funcs->import_bmp(&context, image_data, image_data_size);
    sdb_out_info(__FILE__, __LINE__, ">> analyze start");
    int ret = funcs->start(&context, type);
    sdb_out_info(__FILE__, __LINE__, "<< analyze finish");
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


