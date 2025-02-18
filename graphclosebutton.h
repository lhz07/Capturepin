#ifndef GRAPHCLOSEBUTTON_H
#define GRAPHCLOSEBUTTON_H

#include <QObject>
#include <QGraphicsItem>

class GraphCloseButton : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit GraphCloseButton(QGraphicsItem *parent = nullptr);
    ~GraphCloseButton();
protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    QRectF boundingRect() const override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
signals:
    void clicked();
};

#endif // GRAPHCLOSEBUTTON_H
