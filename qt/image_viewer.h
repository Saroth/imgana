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
    ~MarkPoint() { };

    void set_point(int xpos, int ypos,
            QColor color = QColor(0xFF, 0xFF, 0xFF), qreal width = 1);
    QPoint point(void) const { return po; }
    QPen pen(void) const { return pe; }

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
    ~MarkLine() { };

    void set_line(QLine line,
            QColor color = QColor(0xFF, 0xFF, 0xFF), qreal width = 1);
    QLine line(void) const { return li; }
    QPen pen(void) const { return pe; }


private:
    QLine li;
    QPen pe;

};

class ImagePainter : public QLabel
{
    Q_OBJECT

public:
    explicit ImagePainter(QWidget *parent = 0);
    ~ImagePainter() { };
    void set_scale(double s);
    void set_pixmap(const QPixmap &pixmap);
    void set_text(const QString &text);

    QVector<MarkPoint> points;
    QVector<MarkLine> lines;
    QPoint mouse_pos;
    QPixmap origin_image;

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
    ~ImageViewer() { };

    void set_pixmap(const QPixmap &pixmap = QPixmap(""));
    const QPixmap *pixmap(void);
    const QPixmap *origin_pixmap(void);
    void show_scale(void);
    void remove_scale_by_timer(void);
    void set_scale(double s = 1.0);
    double scale(void);
    bool is_empty(void);
    QPoint mouse_pos(void);

    QVector<MarkPoint> *points(void) { return &painter.points; }
    QVector<MarkLine> *lines(void) { return &painter.lines; }

protected:
    bool eventFilter(QObject *obj, QEvent *evn);

private:
    ImagePainter painter;
    bool mouse_leftbtn_pressed;
    QPoint mouse_last_pos;
    double image_scale_exact;
    QLabel *scale_info;
    QTimer *show_scale_timer;

};

#endif /* __IMAGE_VIEWER__ */

