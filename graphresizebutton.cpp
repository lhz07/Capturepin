#include "graphresizebutton.h"
#include <QCursor>
#include <QPainter>
#include <QGraphicsSceneEvent>
#include <QGraphicsScene>
#include <QGraphicsView>

GraphResizeButton::GraphResizeButton(QGraphicsItem *parent)
    : QGraphicsObject{parent}
{
    this->setCursor(QCursor(Qt::SizeFDiagCursor));
    this->setScale(2);
    this->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    this->setAcceptHoverEvents(true);
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
    if (event->button() == Qt::LeftButton){
        emit mouse_press(event);
    }
}

void GraphResizeButton::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    emit mouse_move(event);
}

void GraphResizeButton::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    is_hovering = true;
    this->scene()->views().constFirst()->blockSignals(true);
}

void GraphResizeButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    is_hovering = false;
    this->scene()->views().constFirst()->blockSignals(false);
}

bool GraphResizeButton::hide()
{
    if (!is_hovering){
        this->setVisible(false);
        return true;
    }
    return false;
}
