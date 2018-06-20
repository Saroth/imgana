#include <QApplication>
#include <QDesktopWidget>

#include "main_window.h"
#include "library_loader.h"

static MainWindow *mw = 0;

static int log_to_ui(void *p, const char *file, size_t line, const char *str)
{
    QString s;
    if (file) {
        s += QString("%1:%2  ").arg(file, 20)
            .arg(line, 4, 10, QLatin1Char('0'));
    }
    s += str;
    mw->append_log(s);

    return 0;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QDesktopWidget *d = QApplication::desktop();

    mw = new MainWindow();
    mw->move((d->width() - mw->width()) / 2, (d->height() - mw->height()) / 2);
    mw->show();

    libana_set_debug(log_to_ui, 0);
    mw->load_analyzer();

    return a.exec();
}

