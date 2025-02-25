#include "graphrotatebutton.h"
#include <QCursor>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsView>

GraphRotateButton::GraphRotateButton(QGraphicsItem *parent)
    : QGraphicsObject{parent}
{
    QSvgRenderer cursor_renderer(QString(":/pic/resource/pic/rotate_cursor.svg"));
    QPixmap cursor(15 * 1.6, 15 * 1.6);
    cursor.fill(Qt::transparent);
    QPainter painter(&cursor);
    painter.setRenderHint(QPainter::Antialiasing);
    cursor_renderer.render(&painter);
    cursor.setDevicePixelRatio(1.6);
    // QPixmap cursor1(":/pic/resource/pic/rotate_cursor.png");
    // cursor1.setDevicePixelRatio(1.6);
    // this->setCursor(QCursor(cursor1));
    this->setCursor(QCursor(cursor));
    this->setScale(2);
    this->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    this->setAcceptHoverEvents(true);
    renderer = new QSvgRenderer(QString(":/pic/resource/pic/rotate.svg"), this);
}

void GraphRotateButton::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // painter->drawImage(this->boundingRect(), QImage(":/pic/resource/pic/rotate.svg"));
    painter->setRenderHint(QPainter::Antialiasing);
    renderer->render(painter, this->boundingRect());
}

QRectF GraphRotateButton::boundingRect() const
{
    return QRectF(-5, -15, 10, 10);
}

void GraphRotateButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit mouse_press(event);
}

void GraphRotateButton::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    emit mouse_move(event);
}

void GraphRotateButton::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    is_hovering = true;
    this->scene()->views().constFirst()->blockSignals(true);
}

void GraphRotateButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    is_hovering = false;
    this->scene()->views().constFirst()->blockSignals(false);
}

bool GraphRotateButton::hide()
{
    if (!is_hovering){
        this->setVisible(false);
        return true;
    }
    return false;
}
