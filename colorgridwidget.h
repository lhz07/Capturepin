#ifndef COLORGRIDWIDGET_H
#define COLORGRIDWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>

class ColorGridWidget : public QWidget {
    Q_OBJECT
public:
    explicit ColorGridWidget(const QList<QColor> &colors,
                             QWidget *parent = nullptr);
    const int cols = 7;      // 每行显示6个颜色块
    const int cellSize = 15; // 每个颜色块的边长
    QSize calculateGridSize() const;

signals:
    void colorSelected(const QColor &color);

protected:
    void paintEvent(QPaintEvent *) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

private:
    QList<QColor> m_colors;
    int m_hoveredIndex; // 当前悬停的颜色块索引
    int m_seletedIndex;
};

#endif // COLORGRIDWIDGET_H
