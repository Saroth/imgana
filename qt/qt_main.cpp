#include <QApplication>
#include <QDesktopWidget>

#include "main_window.h"
#include "library_loader.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QDesktopWidget *d = QApplication::desktop();

    MainWindow mw;
    mw.move((d->width() - mw.width()) / 2, (d->height() - mw.height()) / 2);
    mw.show();

#warning "TODO: load analyzer in thread."
    mw.load_analyzer();

    return a.exec();
}

