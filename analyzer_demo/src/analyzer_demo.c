#include <string.h>

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


int analyzer_start(analyzer_context *ctx, int type)
{
    return 0;
}
int analyzer_is_running(analyzer_context *ctx)
{
    return 0;
}
int analyzer_stop(analyzer_context *ctx)
{
    return 0;
}


