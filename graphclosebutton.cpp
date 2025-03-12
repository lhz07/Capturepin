#include "graphclosebutton.h"
#include <QCursor>
#include <QPainter>
#include <QGraphicsSceneEvent>
#include <QGraphicsScene>
#include <QGraphicsView>

GraphCloseButton::GraphCloseButton(QGraphicsItem *parent)
    : QGraphicsObject{parent}
{
    bound_rect = QRectF(-5, -5, 10, 10);
    this->setCursor(QCursor(Qt::PointingHandCursor));
    this->setScale(2);
    this->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    this->setAcceptHoverEvents(true);
    renderer = new QSvgRenderer(QString(":/pic/resource/pic/close.svg"), this);
}

GraphCloseButton::~GraphCloseButton()
{
    this->scene()->views().constFirst()->blockSignals(false);
    // qDebug() << "close button deleted";
}

void GraphCloseButton::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // painter->setRenderHint(QPainter::Antialiasing);
    // qDebug() << painter->renderHints();
    // painter->drawImage(this->boundingRect(), QImage(":/pic/resource/pic/close.svg"));
    renderer->render(painter, this->boundingRect());
}

QRectF GraphCloseButton::boundingRect() const
{
    return bound_rect;
}

void GraphCloseButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton){
        emit clicked();
    }
}

void GraphCloseButton::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    is_hovering = true;
    this->scene()->views().constFirst()->blockSignals(true);
}

void GraphCloseButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    is_hovering = false;
    this->scene()->views().constFirst()->blockSignals(false);
}

bool GraphCloseButton::hide()
{
    if (!is_hovering){
        this->setVisible(false);
        return true;
    }
    return false;
}

