#include <QTimer>
#include <QDateTime>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QPixmap>
#include <QMouseEvent>
#include <QFont>

#include "main_window.h"
#include "version.h"

#define WINDOW_WIDTH_DEF    1200
#define WINDOW_HEIGHT_DEF   640
#define EDITOR_FONT         "Monospace"
#define EDITOR_FONT_SIZE    8

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    resize(WINDOW_WIDTH_DEF, WINDOW_HEIGHT_DEF);

    create_menu_bar();
    create_status_bar();
    create_central();

    ana_stat = analyze_state_none;
    analyze_time = { 0, 0, };
    analyze_timer.reset();
    qRegisterMetaType<size_t>("size_t");
    qRegisterMetaType<QTextCursor>("QTextCursor");
    libana_thread = new AnalyzerThread();
    connect(libana_thread, &AnalyzerThread::output_log,
            this, &MainWindow::output_log);
    connect(libana_thread, &AnalyzerThread::output_mark_point,
            this, &MainWindow::output_mark_point);
    connect(libana_thread, &AnalyzerThread::output_mark_line,
            this, &MainWindow::output_mark_line);
    connect(libana_thread, &AnalyzerThread::finished,
            this, &MainWindow::image_analyze_stop);
    load_analyzer();
}


void MainWindow::create_menu_bar(void)
{
    menu_bar = menuBar();

    QMenu *file = menu_bar->addMenu("&File");
    QAction *load_library = file->addAction("&Load library");
    connect(load_library, &QAction::triggered,
            this, &MainWindow::load_analyzer);
    QAction *unload_library = file->addAction("&Unload library");
    connect(unload_library, &QAction::triggered,
            this, &MainWindow::unload_analyzer);
    QAction *exit = file->addAction("E&xit");
    connect(exit, &QAction::triggered, this, &MainWindow::close);

    QMenu *window = menu_bar->addMenu("&Window");
    QAction *log_toggle = window->addAction("&Log toggle");
    connect(log_toggle, &QAction::triggered, this, &MainWindow::log_toggle);
    QAction *reset_size = window->addAction("&Reset size");
    connect(reset_size, &QAction::triggered, this, &MainWindow::reset_size);

    QMenu *help = menu_bar->addMenu("&Help");
    QAction *about = help->addAction("&About");
    connect(about, &QAction::triggered, this, &MainWindow::about);
    QAction *about_qt = help->addAction("About&Qt");
    connect(about_qt, &QAction::triggered, this, &MainWindow::about_qt);
}

void MainWindow::load_analyzer(void)
{
    if (libana_thread->isRunning()) {
        log_viewer->append("thread is running, abort");
        return;
    }
    libana_thread->set_task(AnalyzerThread::thread_task_load);
    libana_thread->start();
}

void MainWindow::unload_analyzer(void)
{
    if (libana_thread->isRunning()) {
        log_viewer->append("thread is running, abort");
        return;
    }
    libana_thread->analyzer()->unload();
    button_analyze->setDisabled(true);
}

void MainWindow::log_toggle(void)
{
    if (log_viewer->isHidden()) {
        log_viewer->show();
    }
    else {
        log_viewer->hide();
    }
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
    msg.append(QString("version: %1\n").arg(imgana_version_str()));
    if (libana_thread->analyzer()->is_loaded()) {
        msg.append(QString("library version: %1\n")
                .arg(libana_thread->analyzer()->library_version()));
    }
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

    button_open = new QPushButton("&Open");
    button_open->setFixedWidth(button_open->fontMetrics().width(
                button_open->text()));
    connect(button_open, &QPushButton::pressed, this, &MainWindow::open_file);
    button_reload = new QPushButton("&Reload");
    connect(button_reload, &QPushButton::pressed,
            this, &MainWindow::image_reload);
    button_unload = new QPushButton("&Unload");
    connect(button_unload, &QPushButton::pressed,
            this, &MainWindow::image_unload);
    button_unload->setDisabled(true);
    button_analyze = new QPushButton("&Analyze");
    connect(button_analyze, &QPushButton::pressed,
            this, &MainWindow::image_analyze_start);
    button_analyze->setDisabled(true);
    button_stop = new QPushButton("&Stop");
    connect(button_stop, &QPushButton::pressed,
            this, &MainWindow::image_analyze_stop);
    button_stop->setDisabled(true);
    editor_file_path = new QLineEdit(QString(file_path + "04-1.bmp"));
    editor_file_path->setPlaceholderText("path of image file.");
    editor_file_path->setTextMargins(0, 0, button_open->width(), 0);
    QLabel *label_file_path = new QLabel("&Path:");
    label_file_path->setBuddy(editor_file_path);
    state_viewer = new QTextEdit();
    QPalette state_pal;
    state_pal.setColor(QPalette::Base, QColor(0xF8, 0xF8, 0xF8));
    state_viewer->setPalette(state_pal);
    state_viewer->setFont(QFont(EDITOR_FONT, EDITOR_FONT_SIZE));
    state_viewer->setMinimumWidth(
            state_viewer->fontMetrics().width(QChar('x')) * 32);
    state_viewer->setMaximumHeight(120);
    state_viewer->setReadOnly(true);
    log_viewer = new QTextEdit();
    QPalette log_pal;
    log_pal.setColor(QPalette::Base, QColor(0x20, 0x20, 0x20));
    log_pal.setColor(QPalette::Text, QColor(0xE0, 0xE0, 0xE0));
    log_viewer->setPalette(log_pal);
    log_viewer->setFont(QFont(EDITOR_FONT, EDITOR_FONT_SIZE));
    int w = log_viewer->fontMetrics().width(QChar('x')) * 100;
    log_viewer->setMinimumWidth(w);
    log_viewer->setMaximumWidth(w * 2);
    log_viewer->setReadOnly(true);
    // log_viewer->hide();

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
    button_reload->setFocus();

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
    if (image.load(file) == false) {
        QMessageBox::warning(this, "Error",
                QString("load failed.\ninvalid image file: %1").arg(file));
        return;
    }
    image_viewer.set_pixmap(QPixmap::fromImage(image));
    int ret;
    if ((ret = libana_thread->set_image(file)) < 0) {
        QMessageBox::warning(this, "Error",
                QString("read file failed.\nreturn -0x%1").arg(-ret, 0, 16));
        return;
    }

    button_unload->setEnabled(true);
    if (libana_thread->analyzer()->is_loaded()) {
        button_analyze->setEnabled(true);
        ana_stat = analyze_state_none;
    }
    update_state();
}

void MainWindow::image_unload(void)
{
    image_viewer.set_pixmap(QPixmap(""));
    button_unload->setDisabled(true);
    button_analyze->setDisabled(true);
    update_state();
}

void MainWindow::image_analyze_start(void)
{
    if (libana_thread->isRunning()) {
        return;
    }
    image_viewer.clear_marks();
    editor_file_path->setDisabled(true);
    button_open->setDisabled(true);
    button_reload->setDisabled(true);
    button_unload->setDisabled(true);
    button_analyze->setDisabled(true);
    button_stop->setEnabled(true);
    ana_stat = analyze_state_running;

    libana_thread->set_task(AnalyzerThread::thread_task_analyze);
    analyze_timer.reset();
    analyze_timer.start();
    libana_thread->start();
    update_state();
}

void MainWindow::image_analyze_stop(void)
{
    button_stop->setDisabled(true);
    if (libana_thread->isRunning()) {
        libana_thread->stop();
        ana_stat = analyze_state_stopping;
    }
    else {
        analyze_time = analyze_timer.stop();
        editor_file_path->setEnabled(true);
        button_open->setEnabled(true);
        button_reload->setEnabled(true);
        if (!image_viewer.is_empty()) {
            button_unload->setEnabled(true);
            button_analyze->setEnabled(true);
        }
        if (ana_stat == analyze_state_stopping) {
            ana_stat = analyze_state_stopped;
        }
        else {
            ana_stat = analyze_state_finish;
        }
    }
    update_state();
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
    state_viewer->append(QString("Image size: %1x%2")
            .arg(pix->width()).arg(pix->height()));
    pix = image_viewer.pixmap();
    state_viewer->append(QString("Scale size: %1x%2(x%3)")
            .arg(pix->width()).arg(pix->height()).arg(image_viewer.scale()));
    QPoint pos = image_viewer.mouse_pos();
    state_viewer->append(QString("Point:      %1, %2")
            .arg(pos.x()).arg(pos.y()));
    int r, g, b;
    QColor color = image.pixelColor(pos);
    color.getRgb(&r, &g, &b);
    state_viewer->append(QString("Pixel:      %1,%2,%3(RGB)")
            .arg(r, 2, 16, QLatin1Char('0'))
            .arg(g, 2, 16, QLatin1Char('0'))
            .arg(b, 2, 16, QLatin1Char('0')));
    if (libana_thread->task() == AnalyzerThread::thread_task_analyze) {
        const QString state_info[] = {
            "ready",
            "running",
            "finish",
            "stopping",
            "stopped",
        };
        state_viewer->append(QString("state:      %1")
                .arg(state_info[ana_stat]));
        if (ana_stat == analyze_state_stopped
                || ana_stat == analyze_state_finish) {
            state_viewer->append(QString("marks:      point(x%1), line(x%2)")
                    .arg(image_viewer.points()->size())
                    .arg(image_viewer.lines()->size()));
            state_viewer->append(QString("time used:  %1.%2.%3.%4(ns)")
                    .arg(analyze_time.tv_sec)
                    .arg((analyze_time.tv_nsec / 1000000) % 1000,
                        3, 10, QLatin1Char('0'))
                    .arg((analyze_time.tv_nsec / 1000) % 1000,
                        3, 10, QLatin1Char('0'))
                    .arg(analyze_time.tv_nsec % 1000,
                        3, 10, QLatin1Char('0')));
        }
    }
}


int MainWindow::output_log(void *p, const char *file, size_t line, QString str)
{
    QString s;
    if (file) {
        if (analyze_timer.is_running()) {
            analyze_time = analyze_timer.time();
            s.append(QString("%1.%2.%3.%4 ").arg(analyze_time.tv_sec)
                    .arg((analyze_time.tv_nsec / 1000000) % 1000,
                        3, 10, QLatin1Char('0'))
                    .arg((analyze_time.tv_nsec / 1000) % 1000,
                        3, 10, QLatin1Char('0'))
                    .arg(analyze_time.tv_nsec % 1000,
                        3, 10, QLatin1Char('0')));
        }
        // s.append(QString("%1:").arg((size_t)p, 0, 16));
        s.append(QString("%1:").arg(file, 20));
        s.append(QString("%1  ").arg(line, 4, 10, QLatin1Char('0')));
    }
    s.append(str);
    log_viewer->append(s);
    return 0;
}

int MainWindow::output_mark_point(void *p,
        size_t x, size_t y, size_t width, int r, int g, int b)
{
    MarkPoint poi(x, y, QColor(r, g, b), width);
    image_viewer.points()->append(poi);
    image_viewer.update();
    return 0;
}

int MainWindow::output_mark_line(void *p,
        size_t x1, size_t y1, size_t x2, size_t y2, size_t width,
        int r, int g, int b)
{
    MarkLine lin(QLine(x1, y1, x2, y2), QColor(r, g, b), width);
    image_viewer.lines()->append(lin);
    image_viewer.update();
    return 0;
}

