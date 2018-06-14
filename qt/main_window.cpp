#include "main_window.h"

#include <QTimer>
#include <QDateTime>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    resize(640, 480);

    create_menu_bar();
    create_status_bar();
    create_central();
}

void MainWindow::create_menu_bar(void)
{
    menu_bar = this->menuBar();

    QMenu *file = menu_bar->addMenu(tr("&File"));
    QAction *exit = file->addAction(tr("E&xit"));
    connect(exit, &QAction::triggered, this, &MainWindow::close);

    QMenu *help = menu_bar->addMenu(tr("&Help"));
    QAction *about = help->addAction(tr("&About"));
    connect(about, &QAction::triggered, this, &MainWindow::about);
}

void MainWindow::about(void)
{
    QMessageBox::information(0, tr("About"), tr("This is a Qt test program."));
}

void MainWindow::create_status_bar(void)
{
    status_bar = this->statusBar();

    status_bar_time = new QLabel();
    status_bar_update_time();
    status_bar->addPermanentWidget(status_bar_time);
    QTimer *timer = new QTimer();
    timer->start(1000);
    connect(timer, &QTimer::timeout, this, &MainWindow::status_bar_update_time);

    status_bar->showMessage("Ready", 3000);
}

void MainWindow::status_bar_update_time(void)
{
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    status_bar_time->setText(time);
}

void MainWindow::create_central(void)
{
    // setCentralWidget(...);
}

