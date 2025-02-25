#include "graphview.h"
#include <QMouseEvent>

GraphView::GraphView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent) {
    // this->setMouseTracking(true);
    this->setCursor(Qt::IBeamCursor);
    // qDebug() << "graphview initialized!";
}

void GraphView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton){
        QGraphicsItem* item = this->scene()->itemAt(event->scenePosition(), this->transform());
        if (!item){
            emit mouse_clicked(event);
        }
        // qDebug() << "graphview clicked!";
    }
    // this->setCursor(Qt::PointingHandCursor);
    // qDebug() << "view mouse pressed!";
    QGraphicsView::mousePressEvent(event);
}

void GraphView::mouseMoveEvent(QMouseEvent *event)
{
    // if (!is_hovering){
    //     qDebug() << "mouse is moving!";
    // }
    // qDebug() << "view mouse moved!";
    // this->viewport()->setCursor(Qt::CrossCursor);
    QGraphicsView::mouseMoveEvent(event);
}

void GraphView::mouseReleaseEvent(QMouseEvent *event)
{
    // qDebug() << "mouse button released";
    // this->setCursor(Qt::IBeamCursor);
    QGraphicsView::mouseReleaseEvent(event);
}

void GraphView::text_tool_visible(bool status)
{
    if (status){
        // new_text = true;
        this->show();

    }else{
        // new_text = false;
        this->hide();
    }
}
