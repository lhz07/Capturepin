#ifndef GRAPHRESIZEBUTTON_H
#define GRAPHRESIZEBUTTON_H

#include <QGraphicsObject>
#include <QObject>

class GraphResizeButton : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit GraphResizeButton(QGraphicsItem *parent = nullptr);
protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    QRectF boundingRect() const override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    // void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
signals:
    void mouse_press(QGraphicsSceneMouseEvent *event);
    void mouse_move(QGraphicsSceneMouseEvent *event);
    // void mouse_release(QGraphicsSceneMouseEvent *event);
};

#endif // GRAPHRESIZEBUTTON_H
