#include "graphresizebutton.h"
#include <qcursor.h>
#include <qgraphicssceneevent.h>
#include <qpainter.h>

GraphResizeButton::GraphResizeButton(QGraphicsItem *parent)
    : QGraphicsObject{parent}
{
    this->setCursor(QCursor(Qt::SizeFDiagCursor));
    this->setScale(2);
    this->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
}

void GraphResizeButton::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHints(QPainter::Antialiasing);
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(1);
    painter->setPen(pen);
    painter->setBrush(Qt::white);
    // painter->setBrush(Qt::white);
    // painter->drawImage(resize_area(), QImage(":/pic/resource/pic/resize.svg"));
    painter->drawRoundedRect(this->boundingRect(), 3, 3);
}

QRectF GraphResizeButton::boundingRect() const
{
    return QRectF(-4, -4, 8, 8);
}

void GraphResizeButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit mouse_press(event);
}

void GraphResizeButton::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    emit mouse_move(event);
}
