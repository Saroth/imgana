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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow() { };

    void append_log(QString str);

public slots:
    void load_analyzer(void);

protected:
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *evn);
    void wheelEvent(QWheelEvent *evn);
    void update_state(void);

private:
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

