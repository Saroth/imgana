#include <stdio.h>
#include <stdarg.h>
#include <QMutexLocker>

#include "analyzer_thread.h"


AnalyzerThread::AnalyzerThread(QObject *parent)
    : QThread(parent)
{
    libana = new LibraryLoader();
    libana->set_debug(cb_output_log, this);
    task = thread_task_load;
    // printf("thead inited.\n");
}

AnalyzerThread::~AnalyzerThread()
{
    stop();
    quit();
    // printf("thead quit.\n");
}


int AnalyzerThread::sdb_out_info(const char *file, size_t line,
        const char *fmt, ...)
{
    char buf[1024];
    va_list va;
    va_start(va, fmt);
    vsnprintf(buf, sizeof(buf), fmt, va);
    va_end(va);

    file = strrchr(file, '\\') ? (strrchr(file, '\\') + 1) :
        strrchr(file, '/') ? (strrchr(file, '/') + 1) : file;
    return emit output_log(this, file, line, QString(buf));
}

int AnalyzerThread::cb_output_log(void *p,
        const char *file, size_t line, const char *str)
{
    AnalyzerThread *th_ana = (AnalyzerThread *)p;
    return th_ana->sdb_out_info(file, line, str);
}
int AnalyzerThread::cb_output_mark_point(void *p,
        size_t x, size_t y, size_t width, int r, int g, int b)
{
    return 0;
}
int AnalyzerThread::cb_output_mark_line(void *p,
        size_t x1, size_t y1, size_t x2, size_t y2, size_t width,
        int r, int g, int b)
{
    return 0;
}


void AnalyzerThread::set_task(enum thread_task task)
{
    this->task = task;
}

LibraryLoader *AnalyzerThread::analyzer()
{
    return libana;
}

void AnalyzerThread::load_library()
{
    sdb_out_info(__FILE__, __LINE__, "load analyzer library...");
    int ret = libana->load();
    if (ret == 0 && libana->is_loaded()) {
        sdb_out_info(__FILE__, __LINE__, "analyzer is ready.");
    }
    else {
        sdb_out_info(__FILE__, __LINE__, "load failed! return:%x", -ret);
    }
}

void AnalyzerThread::analyze_image()
{
}

void AnalyzerThread::stop()
{
    libana->stop();
}

void AnalyzerThread::run()
{
    if (task == thread_task_load) {
        load_library();
    }
    else if (task == thread_task_analyze) {
        analyze_image();
    }
    else {
        sdb_out_info(__FILE__, __LINE__, "unknown task: %d", task);
    }
}


