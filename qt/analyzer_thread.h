#ifndef __ANALYZER_THREAD_H__
#define __ANALYZER_THREAD_H__

#include <QThread>

#include "library_loader.h"

class AnalyzerThread : public QThread
{
    Q_OBJECT

public:
    enum thread_task {
        thread_task_load,
        thread_task_analyze,
    };
    explicit AnalyzerThread(QObject *parent = 0);
    ~AnalyzerThread();

    LibraryLoader *analyzer();
    int set_image(const QString filename);
    void set_task(enum thread_task task);
    AnalyzerThread::thread_task task();
    void stop();

protected:
    virtual void run() Q_DECL_OVERRIDE;

private:
    enum thread_task task_id;
    LibraryLoader *libana;

    void load_library();
    void analyze_image();

    int log_output(const char *file, size_t line, const char *fmt, ...);
    int sdb_out_mark_point(size_t x, size_t y,
            size_t width, int r, int g, int b);
    int sdb_out_mark_line(size_t x1, size_t y1, size_t x2, size_t y2,
            size_t width, int r, int g, int b);
    static int cb_output_mark_point(void *p,
            size_t x, size_t y, size_t width, int r, int g, int b);
    static int cb_output_mark_line(void *p,
            size_t x1, size_t y1, size_t x2, size_t y2, size_t width,
            int r, int g, int b);

signals:
    int output_mark_point(void *p,
            size_t x, size_t y, size_t width, int r, int g, int b);
    int output_mark_line(void *p,
            size_t x1, size_t y1, size_t x2, size_t y2, size_t width,
            int r, int g, int b);

};


#endif /* __ANALYZER_THREAD_H__ */

