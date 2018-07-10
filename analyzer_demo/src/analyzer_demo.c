#include <string.h>
#include <unistd.h>

#include "libanalyzer.h"

void analyzer_init(analyzer_context *ctx)
{
    memset(ctx, 0, sizeof(analyzer_context));
}
void analyzer_free(analyzer_context *ctx)
{
    memset(ctx, 0, sizeof(analyzer_context));
}


void analyzer_set_memory_alloc(analyzer_context *ctx,
        func_analyzer_bio_alloc f_alloc, func_analyzer_bio_free f_free,
        void *p)
{
    ctx->f_alloc = f_alloc;
    ctx->f_free = f_free;
    ctx->p_alloc = p;
}
void analyzer_set_debug(analyzer_context *ctx,
        func_analyzer_bio_debug f, void *p)
{
    ctx->f_debug = f;
    ctx->p_debug = p;
}
void analyzer_set_mark_point(analyzer_context *ctx,
        func_analyzer_bio_mark_point f_mark_point, void *p)
{
    ctx->f_mark_point = f_mark_point;
    ctx->p_mark_point = p;
}
void analyzer_set_mark_line(analyzer_context *ctx,
        func_analyzer_bio_mark_line f_mark_line, void *p)
{
    ctx->f_mark_line = f_mark_line;
    ctx->p_mark_line = p;
}


int analyzer_import_bitmap(analyzer_context *ctx,
        const unsigned char *data, size_t size, size_t bits_per_pixel,
        size_t w, size_t h)
{
    return 0;
}
int analyzer_import_bmp(analyzer_context *ctx,
        const unsigned char *bmp, size_t size)
{
    return 0;
}
int analyzer_export_bitmap(analyzer_context *ctx,
        unsigned char *buf, size_t size, size_t *w, size_t *h, size_t *len)
{
    return 0;
}
int analyzer_export_data(analyzer_context *ctx,
        unsigned char *buf, size_t size, size_t *len)
{
    return 0;
}


static int run_state = 0;
int analyzer_start(analyzer_context *ctx, int type)
{
    run_state = 1;
    if (ctx->f_debug) {
        ctx->f_debug(ctx->p_debug, __FILE__, __LINE__, "demo debug.");
    }
    if (ctx->f_mark_point) {
        if (ctx->f_debug) {
            ctx->f_debug(ctx->p_debug, __FILE__, __LINE__, "set mark points.");
        }
        ctx->f_mark_point(ctx->p_mark_point, 1, 1, 3, 0, 0, 0);
        sleep(1);
        ctx->f_mark_point(ctx->p_mark_point, 1, 5, 3, 0xFF, 0, 0);
        sleep(1);
        ctx->f_mark_point(ctx->p_mark_point, 5, 5, 3, 0, 0xFF, 0);
        sleep(1);
        ctx->f_mark_point(ctx->p_mark_point, 5, 1, 3, 0, 0, 0xFF);
        sleep(1);
    }
    if (ctx->f_mark_line) {
        if (ctx->f_debug) {
            ctx->f_debug(ctx->p_debug, __FILE__, __LINE__, "set mark lines.");
        }
        ctx->f_mark_line(ctx->p_mark_line, 2, 2, 4, 4, 1, 0xFF, 0xFF, 0xFF);
        sleep(1);
        ctx->f_mark_line(ctx->p_mark_line, 2, 4, 4, 2, 1, 0xFF, 0xFF, 0xFF);
        sleep(1);
    }
    run_state = 0;
    sleep(1);
    return 0;
}
int analyzer_is_running(analyzer_context *ctx)
{
    return run_state;
}
int analyzer_stop(analyzer_context *ctx)
{
    return 0;
}



extern unsigned int analyzer_version(void);
extern const char *analyzer_version_str(void);

const libana_functions funcs = {
    .set_memory_alloc   = analyzer_set_memory_alloc,
    .set_debug          = analyzer_set_debug,
    .set_mark_point     = analyzer_set_mark_point,
    .set_mark_line      = analyzer_set_mark_line,
    .init               = analyzer_init,
    .free               = analyzer_free,
    .import_bmp         = analyzer_import_bmp,
    .import_bitmap      = analyzer_import_bitmap,
    .export_bitmap      = analyzer_export_bitmap,
    .export_data        = analyzer_export_data,
    .start              = analyzer_start,
    .is_running         = analyzer_is_running,
    .stop               = analyzer_stop,
    .version            = analyzer_version,
    .version_str        = analyzer_version_str,
};

const libana_functions *libana_ioctl(void)
{
    return &funcs;
}


