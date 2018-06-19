#ifndef __LIBRARY_LOADER_H__
#define __LIBRARY_LOADER_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include "libanalyzer.h"

typedef enum {
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
} lib_func_type;
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
extern libana_functions *libana;

int libana_init(void);

const char *libana_version_str(void);

#if defined(__cplusplus)
}
#endif
#endif /* __LIBRARY_LOADER_H__ */

