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
}

void ImagePainter::set_pixmap(const QPixmap &pixmap)
{
    image_scale = 1.0;
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
    }
}


