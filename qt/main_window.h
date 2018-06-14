#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow() { };

private:
    QMenuBar *menu_bar;
    QStatusBar *status_bar;
    QLabel *status_bar_time;

    void create_menu_bar(void);
    void create_status_bar(void);
    void create_central(void);

private slots:
    void about(void);
    void status_bar_update_time(void);

};

#endif /* __MAINWINDOW_H__ */

