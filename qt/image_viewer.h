#ifndef __IMAGE_VIEWER__
#define __IMAGE_VIEWER__

#include <QLabel>
#include <QScrollArea>
#include <QEvent>
#include <QPoint>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QTimer>


class MarkPoint
{
public:
    MarkPoint() { };
    MarkPoint(int xpos, int ypos,
            QColor color = QColor(0xFF, 0xFF, 0xFF), qreal width = 1) {
        set_point(xpos, ypos, color, width);
    };

    void set_point(int xpos, int ypos,
            QColor color = QColor(0xFF, 0xFF, 0xFF), qreal width = 1);
    QPoint point() const { return po; }
    QPen pen() const { return pe; }

private:
    QPoint po;
    QPen pe;

};

class MarkLine
{
public:
    MarkLine() { };
    MarkLine(QLine line,
            QColor color = QColor(0xFF, 0xFF, 0xFF), qreal width = 1) {
        set_line(line, color, width);
    };

    void set_line(QLine line,
            QColor color = QColor(0xFF, 0xFF, 0xFF), qreal width = 1);
    QLine line() const { return li; }
    QPen pen() const { return pe; }


private:
    QLine li;
    QPen pe;

};

class ImagePainter : public QLabel
{
    Q_OBJECT

public:
    explicit ImagePainter(QWidget *parent = 0);
    void set_scale(double s);
    void set_pixmap(const QPixmap &pixmap);
    void set_text(const QString &text);

    QVector<MarkPoint> points;
    QVector<MarkLine> lines;
    QPoint mouse_pos;
    QPixmap origin_image;
    bool show_border;
    QLine pixel_border_top;
    QLine pixel_border_bottom;
    QLine pixel_border_left;
    QLine pixel_border_right;

protected:
    void paintEvent(QPaintEvent *evn);
    bool event(QEvent *evn);

private:
    double image_scale;

private slots:

};

class ImageViewer : public QScrollArea
{
    Q_OBJECT

public:
    explicit ImageViewer(QWidget *parent = 0);

    void set_pixmap(const QPixmap &pixmap = QPixmap(""));
    const QPixmap *pixmap();
    const QPixmap *origin_pixmap();
    void show_scale();
    void remove_scale_by_timer();
    void set_scale(double s = 1.0);
    double scale();
    bool is_empty();
    QPoint mouse_pos();
    void clear_marks();
    void update();

    QVector<MarkPoint> *points() { return &image_painter.points; }
    QVector<MarkLine> *lines() { return &image_painter.lines; }

protected:
    bool eventFilter(QObject *obj, QEvent *evn);

private:
    ImagePainter image_painter;
    bool mouse_leftbtn_pressed;
    QPoint mouse_last_pos;
    double image_scale_exact;
    QLabel *scale_info;
    QTimer *show_scale_timer;

};

#endif /* __IMAGE_VIEWER__ */

