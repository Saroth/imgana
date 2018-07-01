#ifndef __LIBRARY_LOADER_H__
#define __LIBRARY_LOADER_H__

#include "libanalyzer.h"

class LibraryLoader
{
public:
    LibraryLoader();
    ~LibraryLoader();

    enum libana_err_t {
        LIBANA_ERR_LOAD_LIBRARY_FAILED      = -0x200,
        LIBANA_ERR_SYMBOL_IS_NOT_FOUND      = -0x201,
        LIBANA_ERR_LIBRARY_NOT_LOADED       = -0x202,
        LIBANA_ERR_IMAGE_NOT_LOADED         = -0x203,
        LIBANA_ERR_OPEN_FAILED              = -0x204,
        LIBANA_ERR_FSTAT_FAILED             = -0x205,
        LIBANA_ERR_NOT_REGULAR_FILE         = -0x206,
        LIBANA_ERR_READ_FILE                = -0x207,
        LIBANA_ERR_BAD_SIZE                 = -0x208,
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
    const libana_functions *funcs;
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

