#include "graphclosebutton.h"
#include <qcursor.h>
#include <qgraphicssceneevent.h>
#include <qpainter.h>

GraphCloseButton::GraphCloseButton(QGraphicsItem *parent)
    : QGraphicsObject{parent}
{
    bound_rect = QRectF(-5, -5, 10, 10);
    this->setCursor(QCursor(Qt::PointingHandCursor));
    this->setScale(2);
    this->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    renderer = new QSvgRenderer(QString(":/pic/resource/pic/close.svg"), this);
}

GraphCloseButton::~GraphCloseButton() {}

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
