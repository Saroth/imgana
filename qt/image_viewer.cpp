#include "image_viewer.h"

#include <QMouseEvent>
#include <QScrollBar>
#include <math.h>

#define DEFAULT_TEXT "No image loaded."
#define SCALE_MAX 12.0
#define SCALE_MIN 0.2

ImageViewer::ImageViewer(QWidget *parent) : QScrollArea(parent)
{
    mouse_leftbtn_pressed = false;
    image_scale = 1.0;
    painter = new ImagePainter();
    painter->set_text(DEFAULT_TEXT);
    setWidget(painter);
    setBackgroundRole(QPalette::Dark);
    setAlignment(Qt::AlignCenter);

    scale_info = new QLabel("", this);
    QFont fnt;
    fnt.setPixelSize(20);
    scale_info->setFont(fnt);
    show_scale_timer = new QTimer();
    connect(show_scale_timer, &QTimer::timeout,
            this, &ImageViewer::remove_scale_by_timer);
}

void ImageViewer::show_scale(void)
{
    scale_info->setText(QString("x%1").arg(image_scale, 0, 'f', 1));
    scale_info->resize(scale_info->fontMetrics().size(0, scale_info->text()));

    QPoint pos = QPoint(width() - scale_info->width() - 4, 4);
    if (verticalScrollBar()->isVisible()) {
        pos.setX(pos.x() - verticalScrollBar()->width());
    }
    scale_info->move(pos);
    show_scale_timer->start(2000);
}

void ImageViewer::remove_scale_by_timer(void)
{
    show_scale_timer->stop();
    scale_info->setText("");
}

void ImageViewer::set_pixmap(const QPixmap &pixmap)
{
    image_scale = 1.0;
    painter->set_pixmap(pixmap);
    if (is_empty()) {
        painter->set_text(DEFAULT_TEXT);
    }
}

const QPixmap *ImageViewer::pixmap(void)
{
    return painter->pixmap();
}

void ImageViewer::set_scale(double s)
{
    image_scale = s;
    show_scale();
    painter->set_scale(s);
}

double ImageViewer::scale(void)
{
    return image_scale;
}

bool ImageViewer::is_empty(void)
{
    return painter->pixmap() == 0 || painter->pixmap()->isNull();
}


bool ImageViewer::eventFilter(QObject *obj, QEvent *evn)
{
    if (evn->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mos = (QMouseEvent *)evn;
        mouse_last_pos = mos->globalPos();
        if (verticalScrollBar()->isVisible()) {
            if (mouse_last_pos.x() >= verticalScrollBar()->mapToGlobal(
                        verticalScrollBar()->pos()).x()) {
                return verticalScrollBar()->eventFilter(obj, evn);
            }
        }
        if (horizontalScrollBar()->isVisible()) {
            if (mouse_last_pos.y() >= horizontalScrollBar()->mapToGlobal(
                        horizontalScrollBar()->pos()).y()) {
                return horizontalScrollBar()->eventFilter(obj, evn);
            }
        }
        if (mos->buttons() & Qt::LeftButton) {
            mouse_leftbtn_pressed = true;
            return true;
        }
    }
    else if (evn->type() == QEvent::MouseMove) {
        if (mouse_leftbtn_pressed) {
            QPoint mouse_pos_new = ((QMouseEvent *)evn)->globalPos();
            int ofs_x = mouse_pos_new.x() - mouse_last_pos.x();
            int ofs_y = mouse_pos_new.y() - mouse_last_pos.y();
            mouse_last_pos = mouse_pos_new;
            QScrollBar *sclbar;
            sclbar = horizontalScrollBar();
            sclbar->setValue(sclbar->value() - ofs_x);
            sclbar = verticalScrollBar();
            sclbar->setValue(sclbar->value() - ofs_y);
            return true;
        }
    }
    else if (evn->type() == QEvent::MouseButtonRelease) {
        mouse_leftbtn_pressed = false;
    }
    else if (evn->type() == QEvent::Wheel) {
        if (is_empty()) {
            return QObject::eventFilter(obj, evn);
        }

        QWheelEvent *whl = (QWheelEvent *)evn;
        QPoint pos = whl->globalPos();
        if (verticalScrollBar()->isVisible()) {
            if (pos.x() >= verticalScrollBar()->mapToGlobal(
                        verticalScrollBar()->pos()).x()) {
                return verticalScrollBar()->eventFilter(obj, evn);
            }
        }
        if (horizontalScrollBar()->isVisible()) {
            if (pos.y() >= horizontalScrollBar()->mapToGlobal(
                        horizontalScrollBar()->pos()).y()) {
                return horizontalScrollBar()->eventFilter(obj, evn);
            }
        }

        pos = whl->pos();
        if (pos.x() < width() && pos.y() < height()) {
            double imgscl = scale();
            imgscl += (whl->delta() / 120.0 / 10.0) * imgscl;
            imgscl *= 10;
            imgscl = floor(imgscl + 0.5);
            imgscl /= 10;
            if (imgscl < SCALE_MIN) {
                imgscl = SCALE_MIN;
            }
            else if (imgscl > SCALE_MAX) {
                imgscl = SCALE_MAX;
            }

            QSize size_old = painter->pixmap()->size();
            set_scale(imgscl);
            QSize size = painter->size();

            QScrollBar *sclbar = horizontalScrollBar();
            if (sclbar->isVisible()) {
                if (sclbar->maximum() < size.width()) { /* 修复缩小时跳动 */
                    sclbar->setMaximum(size.width());  /* 修复无法对边缘放大 */
                }
                sclbar->setValue((sclbar->value() + pos.x()) * size.width()
                        / size_old.width() - pos.x());
            }
            sclbar = verticalScrollBar();
            if (sclbar->isVisible()) {
                if (sclbar->maximum() < size.height()) {
                    sclbar->setMaximum(size.height());
                }
                sclbar->setValue((sclbar->value() + pos.y()) * size.height()
                        / size_old.height() - pos.y());
            }
            return true;
        }
    }

    return QObject::eventFilter(obj, evn);
}



