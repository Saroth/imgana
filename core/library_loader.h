#ifndef __LIBRARY_LOADER_H__
#define __LIBRARY_LOADER_H__

#include "libanalyzer.h"

class LibraryLoader
{
public:
    LibraryLoader();
    ~LibraryLoader();

    enum libana_err_t {
        LIBANA_ERR_LOAD_LIBRARY_FAILED      = -0x100,
        LIBANA_ERR_SYMBOL_IS_NOT_FOUND      = -0x101,
        LIBANA_ERR_LIBRARY_NOT_LOADED       = -0x102,
        LIBANA_ERR_IMAGE_NOT_LOADED         = -0x103,
        LIBANA_ERR_OPEN_FAILED              = -0x104,
        LIBANA_ERR_FSTAT_FAILED             = -0x105,
        LIBANA_ERR_NOT_REGULAR_FILE         = -0x106,
        LIBANA_ERR_READ_FILE                = -0x107,
        LIBANA_ERR_BAD_SIZE                 = -0x108,
    };
    void set_debug(func_analyzer_bio_debug f, void *p);
    void set_mark_point(func_analyzer_bio_mark_point f, void *p);
    void set_mark_line(func_analyzer_bio_mark_line f, void *p);
    int set_image(const char *file_name);

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
    unsigned char *image_data;
    size_t image_data_size;
    analyzer_context context;
    bool flag_running;

    func_analyzer_bio_debug f_debug;
    void *p_debug;
    func_analyzer_bio_mark_point f_mark_point;
    void *p_mark_point;
    func_analyzer_bio_mark_line f_mark_line;
    void *p_mark_line;

    int sdb_out_info(const char *file, size_t line, const char *fmt, ...);

};

#endif /* __LIBRARY_LOADER_H__ */

