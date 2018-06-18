#ifndef __LIBRARY_LOADER_H__
#define __LIBRARY_LOADER_H__
#if defined(__cplusplus)
extern "C" {
#endif

typedef enum {
    LIBANA_INIT = 0,
    LIBANA_FREE,

    LIBANA_SET_MEMORY_ALLOC,
    LIBANA_SET_DEBUG,
    LIBANA_SET_MARK_POINT,
    LIBANA_SET_MARK_LINE,

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
    void *ptr;
    const char *name;
} libana_function_list;
extern libana_function_list analyzer_func_list[LIBANA_FUNC_MAX];

int libana_init(void);

const char *libana_version_str(void);

#if defined(__cplusplus)
}
#endif
#endif /* __LIBRARY_LOADER_H__ */

