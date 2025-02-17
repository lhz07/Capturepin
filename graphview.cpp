#include "graphview.h"

GraphView::GraphView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent) {
    this->setMouseTracking(true);
    // qDebug() << "graphview initialized!";
}

void GraphView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton){
        emit mouse_clicked(event);
        emit send_mouse_event(event);
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
    emit send_mouse_event(event);
    if (!is_hovering && this->viewport()->cursor() != Qt::IBeamCursor){
        // this->viewport()->setCursor(Qt::IBeamCursor);
    }
    // this->viewport()->setCursor(Qt::CrossCursor);
    QGraphicsView::mouseMoveEvent(event);
}

void GraphView::mouseReleaseEvent(QMouseEvent *event)
{
    // qDebug() << "mouse button released";
    emit send_mouse_event(event);
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

void GraphView::get_hovering(bool status)
{
    this->is_hovering = status;
}
