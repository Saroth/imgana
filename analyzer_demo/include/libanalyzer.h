#ifndef __ANALYZER_DEMO__
#define __ANALYZER_DEMO__
#if defined(__cplusplus)
extern "C" {
#endif

#include <stdlib.h>

typedef void *(*func_analyzer_bio_alloc)(void *p, size_t size);
typedef void (*func_analyzer_bio_free)(void *p, void *addr);
typedef int (*func_analyzer_bio_debug)(void *p, const char *file, size_t line,
        const char *str);
typedef int (*func_analyzer_bio_mark_point)(void *p,
        size_t x, size_t y, size_t width,
        int r, int g, int b);
typedef int (*func_analyzer_bio_mark_line)(void *p,
        size_t x1, size_t y1, size_t x2, size_t y2, size_t width,
        int r, int g, int b);
typedef struct {
    func_analyzer_bio_alloc f_alloc;
    func_analyzer_bio_free f_free;
    void *p_alloc;
    func_analyzer_bio_debug f_debug;
    void *p_debug;
    func_analyzer_bio_mark_point f_mark_point;
    void *p_mark_point;
    func_analyzer_bio_mark_line f_mark_line;
    void *p_mark_line;

    unsigned char flag_run;                     /* start/stop control */
    unsigned char flag_busy;                    /* is_running state */
} analyzer_context;

void analyzer_init(analyzer_context *ctx);
void analyzer_free(analyzer_context *ctx);

void analyzer_set_memory_alloc(analyzer_context *ctx,
        func_analyzer_bio_alloc f_alloc, func_analyzer_bio_free f_free,
        void *p);
void analyzer_set_debug(analyzer_context *ctx,
        func_analyzer_bio_debug f, void *p);
void analyzer_set_mark_point(analyzer_context *ctx,
        func_analyzer_bio_mark_point f_mark_point, void *p);
void analyzer_set_mark_line(analyzer_context *ctx,
        func_analyzer_bio_mark_line f_mark_line, void *p);

int analyzer_import_bmp(analyzer_context *ctx, const char *bmp, size_t size);
int analyzer_export_bitmap(analyzer_context *ctx,
        unsigned char *bitmap, size_t size, size_t *w, size_t *h, size_t *len);
int analyzer_export_data(analyzer_context *ctx,
        unsigned char *data, size_t size, size_t *len);

int analyzer_start(analyzer_context *ctx);
int analyzer_is_running(analyzer_context *ctx);
int analyzer_stop(analyzer_context *ctx);

unsigned int analyzer_version(void);
const char *analyzer_version_str(void);


typedef void (*func_analyzer_init)(analyzer_context *ctx);
typedef void (*func_analyzer_free)(analyzer_context *ctx);
typedef void (*func_analyzer_set_memory_alloc)(analyzer_context *ctx,
        func_analyzer_bio_alloc f_alloc, func_analyzer_bio_free f_free,
        void *p);
typedef void (*func_analyzer_set_debug)(analyzer_context *ctx,
        func_analyzer_bio_debug f, void *p);
typedef void (*func_analyzer_set_mark_point)(analyzer_context *ctx,
        func_analyzer_bio_mark_point f_mark_point, void *p);
typedef void (*func_analyzer_set_mark_line)(analyzer_context *ctx,
        func_analyzer_bio_mark_line f_mark_line, void *p);

typedef int (*func_analyzer_import_bmp)(analyzer_context *ctx,
        const char *bmp, size_t size);
typedef int (*func_analyzer_export_bitmap)(analyzer_context *ctx,
        unsigned char *bitmap, size_t size, size_t *w, size_t *h, size_t *len);
typedef int (*func_analyzer_export_data)(analyzer_context *ctx,
        unsigned char *data, size_t size, size_t *len);

typedef int (*func_analyzer_start)(analyzer_context *ctx);
typedef int (*func_analyzer_is_running)(analyzer_context *ctx);
typedef int (*func_analyzer_stop)(analyzer_context *ctx);

typedef unsigned int (*func_analyzer_version)(void);
typedef const char *(*func_analyzer_version_str)(void);


#if defined(__cplusplus)
}
#endif
#endif /* __ANALYZER_DEMO__ */

