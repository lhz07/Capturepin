#include "colorgridwidget.h"

// ColorGridWidget::ColorGridWidget(QObject *parent)
//     : QObject{parent}
// {}
ColorGridWidget::ColorGridWidget(const QList<QColor> &colors, QWidget *parent)
    : QWidget(parent), m_colors(colors), m_hoveredIndex(-1),
    m_seletedIndex(-1) {
    setFixedSize(calculateGridSize());
    setMouseTracking(true); // 启用鼠标跟踪以检测悬停
}
QSize ColorGridWidget::calculateGridSize() const {
    
    const int rows = (m_colors.size() + cols - 1) / cols;
    
    return QSize(cols * cellSize + (cols - 1) * 2,
                 rows * cellSize + (rows - 1) * 2);
}
void ColorGridWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // const int cellSize = 24;
    const int spacing = 2;
    int index = 0;
    const auto color_list = m_colors;
    
    for (const QColor &color : color_list) {
        int row = index / cols;
        int col = index % cols;
        QRect rect(col * (cellSize + spacing), row * (cellSize + spacing), cellSize,
                   cellSize);
        
        // 绘制颜色块
        painter.setBrush(color);
        painter.setPen(Qt::NoPen);
        // painter.drawEllipse(rect);
        painter.drawRect(rect);
        
        // 高亮悬停状态
        if (index == m_seletedIndex) {
            painter.setPen(QPen(Qt::blue, 2));
            painter.drawRect(rect.adjusted(1, 1, -1, -1));
        } else if (index == m_hoveredIndex) {
            painter.setPen(QPen(Qt::white, 2));
            // painter.drawEllipse(rect.adjusted(1, 1, -1, -1));
            painter.drawRect(rect.adjusted(1, 1, -1, -1));
        }
        
        index++;
    }
}
void ColorGridWidget::mouseMoveEvent(QMouseEvent *event) {
    // 计算鼠标悬停的颜色块索引
    const QPoint pos = event->pos();
    // const int cellSize = 24;
    const int spacing = 2;
    int col = pos.x() / (cellSize + spacing);
    int row = pos.y() / (cellSize + spacing);
    int newHoveredIndex = row * cols + col;
    
    if (newHoveredIndex != m_hoveredIndex && newHoveredIndex < m_colors.size()) {
        m_hoveredIndex = newHoveredIndex;
        update(); // 触发重绘以显示高亮
    }
}
void ColorGridWidget::mousePressEvent(QMouseEvent *event) {
    if (m_hoveredIndex >= 0 && m_hoveredIndex < m_colors.size()) {
        emit colorSelected(m_colors[m_hoveredIndex]);
        m_seletedIndex = m_hoveredIndex;
        update();
    }
}
