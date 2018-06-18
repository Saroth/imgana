#include <QTimer>
#include <QDateTime>
#include <QMessageBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QImage>
#include <QPixmap>
#include <QMouseEvent>

#include <unistd.h>

#include "main_window.h"
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
    menu_bar = menuBar();

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
    status_bar = statusBar();

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
    file_path = "./images/";

    QPushButton *button_open = new QPushButton("&Open");
    button_open->setFixedWidth(button_open->fontMetrics().width(
                button_open->text()));
    connect(button_open, &QPushButton::pressed, this, &MainWindow::open_file);
    QPushButton *button_reload = new QPushButton("&Reload");
    connect(button_reload, &QPushButton::pressed,
            this, &MainWindow::image_reload);
    QPushButton *button_unload = new QPushButton("&Unload");
    connect(button_unload, &QPushButton::pressed,
            this, &MainWindow::image_unload);
    QPushButton *button_analyze = new QPushButton("&Analyze");
    connect(button_unload, &QPushButton::pressed,
            this, &MainWindow::image_analyze);
    QPushButton *button_stop = new QPushButton("&Stop");
    connect(button_unload, &QPushButton::pressed,
            this, &MainWindow::image_analyze_stop);
    editor_file_path = new QLineEdit(QString(file_path + "01-1.jpg"));
    editor_file_path->setPlaceholderText("path of image file.");
    editor_file_path->setTextMargins(0, 0, button_open->width(), 0);
    QLabel *label_file_path = new QLabel("&Path:");
    label_file_path->setBuddy(editor_file_path);
    state_viewer = new QTextEdit();
    state_viewer->setMaximumHeight(100);
    state_viewer->setReadOnly(true);
    QPalette state_pal;
    state_pal.setColor(QPalette::Base, QColor(0xF8, 0xF8, 0xF8));
    state_viewer->setPalette(state_pal);
    log_viewer = new QTextEdit();
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
    layout_main->addWidget(&image_viewer);
    QHBoxLayout *layout_top = new QHBoxLayout();
    layout_top->setMargin(4);
    layout_top->setSpacing(2);
    layout_top->addLayout(layout_main);
    layout_top->addWidget(log_viewer);
    QWidget *central = new QWidget();
    central->setLayout(layout_top);
    setCentralWidget(central);

    image_viewer.setMouseTracking(true);
    central->setMouseTracking(true);
    setMouseTracking(true);
    update_state();
}

void MainWindow::open_file(void)
{
    QString file = QFileDialog::getOpenFileName(this, "open image file",
            file_path, "Image files(*)");
    if (file.length() == 0) {
        return;
    }
    QFileInfo info = QFileInfo(file);
    file_path = info.absolutePath();

    editor_file_path->setText(file);
    image_reload();
}

void MainWindow::image_reload(void)
{
    QString file = editor_file_path->text();
    QImage image;
    if (image.load(file) == false) {
        QString err = "Load failed.\nInvalid image file:" + file;
        QMessageBox::warning(this, "Error", err);
    }
    image_viewer.set_pixmap(QPixmap::fromImage(image));
    update_state();
}

void MainWindow::image_unload(void)
{
    image_viewer.set_pixmap(QPixmap(""));
    update_state();
}

void MainWindow::image_analyze(void)
{
}

void MainWindow::image_analyze_stop(void)
{
    
}


void MainWindow::append_log(QString str)
{
    log_viewer->append(str);
}

void MainWindow::mouseMoveEvent(QMouseEvent *evn)
{
    update_state();
}

void MainWindow::wheelEvent(QWheelEvent *evn)
{
    update_state();
}

void MainWindow::update_state(void)
{
    state_viewer->clear();
    if (image_viewer.is_empty()) {
        return;
    }
    const QPixmap *pix = image_viewer.origin_pixmap();
    state_viewer->append(QString("Image size: \t%1x%2")
            .arg(pix->width()).arg(pix->height()));
    pix = image_viewer.pixmap();
    state_viewer->append(QString("Scale size: \t%1x%2(x%3)")
            .arg(pix->width()).arg(pix->height()).arg(image_viewer.scale()));
    QPoint pos = image_viewer.mouse_pos();
    state_viewer->append(QString("Point: \t%1x%2")
            .arg(pos.x()).arg(pos.y()));
}

