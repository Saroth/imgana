#include <stdio.h>
#include <stdarg.h>
#include <QMutexLocker>

#include "analyzer_thread.h"


AnalyzerThread::AnalyzerThread(QObject *parent)
    : QThread(parent)
{
    libana = new LibraryLoader();
    libana->set_mark_point(cb_output_mark_point, this);
    libana->set_mark_line(cb_output_mark_line, this);
    task_id = thread_task_load;
}

AnalyzerThread::~AnalyzerThread()
{
    stop();
    quit();
}


int AnalyzerThread::log_output(const char *file, size_t line,
        const char *fmt, ...)
{
    char buf[1024];
    va_list va;
    va_start(va, fmt);
    vsnprintf(buf, sizeof(buf), fmt, va);
    va_end(va);
    return libana->log_output(file, line, buf);
}

int AnalyzerThread::cb_output_mark_point(void *p,
        size_t x, size_t y, size_t width, int r, int g, int b)
{
    AnalyzerThread *th_ana = (AnalyzerThread *)p;
    return emit th_ana->output_mark_point(p, x, y, width, r, g, b);
}
int AnalyzerThread::cb_output_mark_line(void *p,
        size_t x1, size_t y1, size_t x2, size_t y2, size_t width,
        int r, int g, int b)
{
    AnalyzerThread *th_ana = (AnalyzerThread *)p;
    return emit th_ana->output_mark_line(p, x1, y1, x2, y2, width, r, g, b);
}


int AnalyzerThread::set_image(const QString filename)
{
    return libana->set_image(filename.toLatin1().data());
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
    log_output(__FILE__, __LINE__, "load analyzer library...");
    int ret = libana->load();
    if (ret == 0 && libana->is_loaded()) {
        log_output(__FILE__, __LINE__, "analyzer is ready.");
    }
    else {
        log_output(__FILE__, __LINE__, "load failed! return:%d(%s%#x)",
                ret, ret < 0 ? "-" : "", ret < 0 ? -ret : ret);
    }
}

void AnalyzerThread::analyze_image()
{
    log_output(__FILE__, __LINE__, "run analyzer, mode:%d", 0);
    int ret = libana->run();
    log_output(__FILE__, __LINE__, "analyzer finished, return:%d(%s%#x)",
            ret, ret < 0 ? "-" : "", ret < 0 ? -ret : ret);
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
        log_output(__FILE__, __LINE__, "unknown task id: %d", task_id);
    }
}


