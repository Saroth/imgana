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
#include "library_loader.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

public slots:
    void load_analyzer(void);

protected:
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *evn);
    void wheelEvent(QWheelEvent *evn);
    void update_state(void);

    void append_log(QString str);
    static int callback_debug(void *p, const char *file, size_t line,
            const char *str);
    static int callback_mark_point(void *p,
            size_t x, size_t y, size_t width, int r, int g, int b);
    static int callback_mark_line(void *p,
            size_t x1, size_t y1, size_t x2, size_t y2, size_t width,
            int r, int g, int b);

private:
    LibraryLoader libana;

    enum analyze_state {
        analyze_state_none,
        analyze_state_running,
        analyze_state_finish,
        analyze_state_stopped,
    };
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
    enum analyze_state ana_stat;

    void create_menu_bar(void);
    void create_status_bar(void);
    void create_central(void);

private slots:
    void log_toggle(void);
    void reset_size(void);
    void about(void);
    void about_qt(void);
    void status_bar_update_time(void);
    void open_file(void);
    void image_reload(void);
    void image_unload(void);
    void image_analyze(void);
    void image_analyze_stop(void);

};

#endif /* __MAINWINDOW_H__ */

