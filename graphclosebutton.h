#ifndef GRAPHCLOSEBUTTON_H
#define GRAPHCLOSEBUTTON_H

#include <QObject>
#include <QGraphicsItem>
#include <QSvgRenderer>

class GraphCloseButton : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit GraphCloseButton(QGraphicsItem *parent = nullptr);
    ~GraphCloseButton();
    bool hide();

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    QRectF boundingRect() const override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
private:
    QSvgRenderer* renderer;
    QRectF bound_rect;
    bool is_hovering = false;

signals:
    void clicked();
};

#endif // GRAPHCLOSEBUTTON_H
