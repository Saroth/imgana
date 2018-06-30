#ifndef __LIBRARY_LOADER_H__
#define __LIBRARY_LOADER_H__

#include "libanalyzer.h"

class LibraryLoader
{
public:
    enum libana_err_t {
        LIBANA_ERR_LOAD_LIBRARY_FAILED      = -0x100,
        LIBANA_ERR_SYMBOL_IS_NOT_FOUND      = -0x101,
        LIBANA_ERR_LIBRARY_NOT_LOADED       = -0x102,
    };
    LibraryLoader();

    void set_debug(int (*f)(void *, const char *, size_t, const char *),
            void *p);

    int load();
    int load(const char *filename);
    void unload();
    bool is_loaded();
    const char *library_version(void);

    int run(int type = 0);
    bool is_running();
    void stop();
    bool is_busy();

private:
    enum lib_func_type {
        LIBANA_INIT = 0,
        LIBANA_FREE,

        LIBANA_SET_MEMORY_ALLOC,
        LIBANA_SET_DEBUG,
        LIBANA_SET_MARK_POINT,
        LIBANA_SET_MARK_LINE,

        LIBANA_IMPORT_BITMAP,
        LIBANA_IMPORT_BMP,
        LIBANA_EXPORT_BITMAP,
        LIBANA_EXPORT_DATA,

        LIBANA_START,
        LIBANA_IS_RUNNING,
        LIBANA_STOP,

        LIBANA_VERSION,
        LIBANA_VERSION_STR,

        LIBANA_FUNC_MAX,
    };
    typedef struct {
        func_analyzer_init                  init;
        func_analyzer_free                  free;
        func_analyzer_set_memory_alloc      set_memory_alloc;
        func_analyzer_set_debug             set_debug;
        func_analyzer_set_mark_point        set_mark_point;
        func_analyzer_set_mark_line         set_mark_line;
        func_analyzer_import_bitmap         import_bitmap;
        func_analyzer_import_bmp            import_bmp;
        func_analyzer_export_bitmap         export_bitmap;
        func_analyzer_export_data           export_data;
        func_analyzer_start                 start;
        func_analyzer_is_running            is_running;
        func_analyzer_stop                  stop;
        func_analyzer_version               version;
        func_analyzer_version_str           version_str;
    } libana_functions;
    const libana_functions *funcs;
    void *func_list[LIBANA_FUNC_MAX];
    void *handler;
    analyzer_context context;
    bool flag_running;

    int (*f_debug)(void *, const char *, size_t, const char *);
    void *p_debug;

    int sdb_out_info(const char *file, size_t line, const char *fmt, ...);

};

#endif /* __LIBRARY_LOADER_H__ */

