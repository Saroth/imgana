#include <QApplication>
#include <QDesktopWidget>

#include "main_window.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QDesktopWidget *d = QApplication::desktop();

    MainWindow mw;
    mw.move((d->width() - mw.width()) / 3, (d->height() - mw.height()) / 2);
    mw.show();
    return a.exec();
}

