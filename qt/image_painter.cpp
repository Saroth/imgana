#include <QLayout>
#include <QMouseEvent>

#include "image_viewer.h"


void MarkPoint::set_point(int xpos, int ypos, QColor color, qreal width)
{
    po = QPoint(xpos, ypos);
    pe = QPen(color, width);
}

void MarkLine::set_line(QLine line, QColor color, qreal width)
{
    li = QLine(line);
    pe = QPen(color, width);
}



ImagePainter::ImagePainter(QWidget *parent) : QLabel(parent)
{
    QFont fnt;
    fnt.setPixelSize(32);
    setFont(fnt);

    points.resize(400);
    lines.resize(64);

    setMouseTracking(true);
}

void ImagePainter::set_pixmap(const QPixmap &pixmap)
{
    origin_image = pixmap;
    resize(origin_image.size());
    setPixmap(origin_image);

    points.clear();
    lines.clear();
}

void ImagePainter::set_scale(double s)
{
    image_scale = s;
    QSize size = QSize(origin_image.width() * s, origin_image.height() * s);
    resize(size);
    setPixmap(origin_image.scaled(size));
    show_border = false;
}

void ImagePainter::set_text(const QString &t)
{
    setText(t);
    resize(fontMetrics().size(0, text()));
}

void ImagePainter::paintEvent(QPaintEvent *evn)
{
    QPainter pai(this);
    // pai.setRenderHint(QPainter::Antialiasing, false);    /* 反锯齿 */
    if (pixmap() == 0 || pixmap()->isNull()) {
        pai.drawText(rect(), Qt::AlignCenter, text());
    }
    else {
        pai.drawPixmap(rect(), *pixmap());
        for (int i = 0; i < points.size(); i++) {
            pai.setPen(points.at(i).pen());
            QPoint poi = points.at(i).point();
            poi.setX((poi.x() + 0.5) * image_scale);
            poi.setY((poi.y() + 0.5) * image_scale);
            pai.drawPoint(poi);
        }
        for (int i = 0; i < lines.size(); i++) {
            pai.setPen(lines.at(i).pen());
            QLine lin = lines.at(i).line();
            lin.setLine((lin.x1() + 0.5) * image_scale,
                    (lin.y1() + 0.5) * image_scale,
                    (lin.x2() + 0.5) * image_scale,
                    (lin.y2() + 0.5) * image_scale);
            pai.drawLine(lin);
        }
        if (show_border) {
            pai.drawLine(pixel_border_top);
            pai.drawLine(pixel_border_bottom);
            pai.drawLine(pixel_border_left);
            pai.drawLine(pixel_border_right);
        }
    }
}

bool ImagePainter::event(QEvent *evn)
{
    if (evn->type() == QMouseEvent::MouseMove
            && pixmap() && !pixmap()->isNull()) {
        mouse_pos = ((QMouseEvent *)evn)->pos();
        mouse_pos.setX((mouse_pos.x() - 1) / image_scale);
        mouse_pos.setY((mouse_pos.y() - 1) / image_scale);

        if (image_scale >= 4) {
            show_border = true;
            int xl = mouse_pos.x() * image_scale - 1;
            int yt = mouse_pos.y() * image_scale - 1;
            int xr = (mouse_pos.x() + 1) * image_scale + 1;
            int yb = (mouse_pos.y() + 1) * image_scale + 1;
            pixel_border_top.setLine(xl, yt, xr, yt);
            pixel_border_bottom.setLine(xl, yb, xr, yb);
            pixel_border_left.setLine(xl, yt, xl, yb);
            pixel_border_right.setLine(xr, yt, xr, yb);
            update();
        }
        else {
            show_border = false;
        }
    }

    return QWidget::event(evn);
}

