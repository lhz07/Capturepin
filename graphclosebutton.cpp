#include "graphclosebutton.h"
#include <qcursor.h>
#include <qpainter.h>

GraphCloseButton::GraphCloseButton(QGraphicsItem *parent)
    : QGraphicsObject{parent}
{
    this->setCursor(QCursor(Qt::PointingHandCursor));
    this->setScale(2);
    this->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
}

void GraphCloseButton::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawImage(this->boundingRect(), QImage(":/pic/resource/pic/close.svg"));
}

QRectF GraphCloseButton::boundingRect() const
{
    return QRectF(-5, -5, 10, 10);
}

void GraphCloseButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit clicked();
}
