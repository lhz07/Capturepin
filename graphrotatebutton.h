#ifndef GRAPHROTATEBUTTON_H
#define GRAPHROTATEBUTTON_H

#include <QObject>
#include <QGraphicsItem>
#include <QSvgRenderer>

class GraphRotateButton : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit GraphRotateButton(QGraphicsItem *parent = nullptr);
    bool hide();

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    QRectF boundingRect() const override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    // void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    QSvgRenderer* renderer;
    bool is_hovering = false;

signals:
    void mouse_press(QGraphicsSceneMouseEvent *event);
    void mouse_move(QGraphicsSceneMouseEvent *event);
    // void mouse_release(QGraphicsSceneMouseEvent *event);
};

#endif // GRAPHROTATEBUTTON_H
