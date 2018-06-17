#include <QTimer>
#include <QDateTime>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "main_window.h"
#include "image_viewer.h"
#include "version.h"

#define WINDOW_WIDTH_DEF    800
#define WINDOW_HEIGHT_DEF   600

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    resize(WINDOW_WIDTH_DEF, WINDOW_HEIGHT_DEF);

    create_menu_bar();
    create_status_bar();
    create_central();
}

void MainWindow::create_menu_bar(void)
{
    menu_bar = this->menuBar();

    QMenu *file = menu_bar->addMenu("&File");
    QAction *exit = file->addAction("E&xit");
    connect(exit, &QAction::triggered, this, &MainWindow::close);

    QMenu *window = menu_bar->addMenu("&Window");
    QAction *reset_size = window->addAction("&Reset size");
    connect(reset_size, &QAction::triggered, this, &MainWindow::reset_size);

    QMenu *help = menu_bar->addMenu("&Help");
    QAction *about = help->addAction("&About");
    connect(about, &QAction::triggered, this, &MainWindow::about);
    QAction *about_qt = help->addAction("About&Qt");
    connect(about_qt, &QAction::triggered, this, &MainWindow::about_qt);
}

void MainWindow::reset_size(void)
{
    resize(WINDOW_WIDTH_DEF, WINDOW_HEIGHT_DEF);
}

void MainWindow::about(void)
{
    QString msg;
    msg.append("Image Analyzer\n");
    msg.append("\n");
    msg.append(QString("version %1\n").arg(imgana_get_version_str()));
    QMessageBox::about(this, "About", msg);
}

void MainWindow::about_qt(void)
{
    QMessageBox::aboutQt(this);
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
}

void MainWindow::status_bar_update_time(void)
{
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    status_bar_time->setText(time);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    status_bar->showMessage(QString("Size:%1x%2").arg(width()).arg(height()),
            3000);
}

void MainWindow::create_central(void)
{
    QPushButton *button_open = new QPushButton("&Open");
    button_open->setFixedWidth(button_open->fontMetrics().width(
                button_open->text()));
    QPushButton *button_reload = new QPushButton("&Reload");
    QPushButton *button_unload = new QPushButton("&Unload");
    QPushButton *button_analyze = new QPushButton("&Analyze");
    QPushButton *button_stop = new QPushButton("&Stop");
    QLineEdit *editor_file_path = new QLineEdit();
    editor_file_path->setPlaceholderText("path of image file.");
    editor_file_path->setTextMargins(0, 0, button_open->width(), 0);
    QLabel *label_file_path = new QLabel("&Path:");
    label_file_path->setBuddy(editor_file_path);
    ImageViewer *image_viewer = new ImageViewer();
    QTextEdit *state_viewer = new QTextEdit();
    state_viewer->setMaximumHeight(100);
    state_viewer->setReadOnly(true);
    state_viewer->append("No image loaded.");
    QPalette state_pal;
    state_pal.setColor(QPalette::Base, QColor(0xC0, 0xC0, 0xC0));
    state_viewer->setPalette(state_pal);
    QTextEdit *log_viewer = new QTextEdit();
    log_viewer->setMaximumWidth(600);
    log_viewer->setMinimumWidth(200);
    log_viewer->setReadOnly(true);
    log_viewer->append("Ready.");
    QPalette log_pal;
    log_pal.setColor(QPalette::Base, QColor(0x20, 0x20, 0x20));
    log_pal.setColor(QPalette::Text, QColor(0xE0, 0xE0, 0xE0));
    log_viewer->setPalette(log_pal);

    QHBoxLayout *layout_embedded_button = new QHBoxLayout();
    layout_embedded_button->setMargin(0);
    layout_embedded_button->setSpacing(0);
    layout_embedded_button->addWidget(button_open, 0, Qt::AlignRight);
    editor_file_path->setLayout(layout_embedded_button);
    QHBoxLayout *layout_file_path = new QHBoxLayout();
    layout_file_path->addWidget(label_file_path);
    layout_file_path->addWidget(editor_file_path);
    QGridLayout *layout_buttons = new QGridLayout();
    layout_buttons->setMargin(8);
    layout_buttons->setSpacing(4);
    layout_buttons->addWidget(button_reload, 0, 0);
    layout_buttons->addWidget(button_unload, 0, 1);
    layout_buttons->addWidget(button_analyze, 1, 0);
    layout_buttons->addWidget(button_stop, 1, 1);
    QHBoxLayout *layout_controller = new QHBoxLayout();
    layout_controller->setMargin(0);
    layout_controller->setSpacing(2);
    layout_controller->addWidget(state_viewer);
    layout_controller->addLayout(layout_buttons);
    QVBoxLayout *layout_main = new QVBoxLayout();
    layout_main->setMargin(0);
    layout_main->setSpacing(2);
    layout_main->addLayout(layout_file_path);
    layout_main->addLayout(layout_controller);
    layout_main->addWidget(image_viewer);
    QHBoxLayout *layout_top = new QHBoxLayout();
    layout_top->setMargin(4);
    layout_top->setSpacing(2);
    layout_top->addLayout(layout_main);
    layout_top->addWidget(log_viewer);
    QWidget *central = new QWidget();
    central->setLayout(layout_top);
    setCentralWidget(central);
}

