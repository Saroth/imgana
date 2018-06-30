#include <stdio.h>
#include <stdarg.h>
#include <QMutexLocker>

#include "analyzer_thread.h"


AnalyzerThread::AnalyzerThread(QObject *parent)
    : QThread(parent)
{
    libana = new LibraryLoader();
    libana->set_debug(cb_output_log, this);
    task_id = thread_task_load;
}

AnalyzerThread::~AnalyzerThread()
{
    stop();
    quit();
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
    task_id = task;
}

AnalyzerThread::thread_task AnalyzerThread::task()
{
    return task_id;
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
    sdb_out_info(__FILE__, __LINE__, "run analyzer, mode:%d", 0);
    int ret = libana->run();
    sdb_out_info(__FILE__, __LINE__, "analyzer finished, return:%d(%s%#x)",
            ret, ret > 0 ? "" : "-", ret > 0 ? ret : -ret);
}

void AnalyzerThread::stop()
{
    libana->stop();
}

void AnalyzerThread::run()
{
    if (task_id == thread_task_load) {
        load_library();
    }
    else if (task_id == thread_task_analyze) {
        analyze_image();
    }
    else {
        sdb_out_info(__FILE__, __LINE__, "unknown task id: %d", task_id);
    }
}


