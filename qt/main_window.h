#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QImage>

#include "image_viewer.h"
#include "analyzer_thread.h"
#include "library_loader.h"
#include "timer.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

protected:
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *evn);
    void wheelEvent(QWheelEvent *evn);
    void update_state();

private:
    enum analyze_state {
        analyze_state_none,
        analyze_state_running,
        analyze_state_finish,
        analyze_state_stopping,
        analyze_state_stopped,
    };
    enum analyze_state ana_stat;
    AnalyzerThread *libana_thread;
    Timer analyze_timer;
    struct timespec analyze_time;

    QImage image;
    QMenuBar *menu_bar;
    QStatusBar *status_bar;
    QLabel *status_bar_time;
    QString file_path;
    QLineEdit *editor_file_path;
    ImageViewer image_viewer;
    QTextEdit *state_viewer;
    QTextEdit *log_viewer;
    QPushButton *button_open;
    QPushButton *button_reload;
    QPushButton *button_unload;
    QPushButton *button_analyze;
    QPushButton *button_stop;

    void create_menu_bar();
    void create_status_bar();
    void create_central();

private slots:
    void load_analyzer();
    void unload_analyzer();
    void log_toggle();
    void reset_size();
    void about();
    void about_qt();
    void status_bar_update_time();
    void open_file();
    void image_reload();
    void image_unload();
    void image_analyze();
    void image_analyze_stop();

    int output_log(void *p, const char *file, size_t line, QString str);
    int output_mark_point(void *p,
            size_t x, size_t y, size_t width, int r, int g, int b);
    int output_mark_line(void *p,
            size_t x1, size_t y1, size_t x2, size_t y2, size_t width,
            int r, int g, int b);


};

#endif /* __MAINWINDOW_H__ */

