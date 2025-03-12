#include "graphview.h"
#include "textbox.h"
#include <QMouseEvent>
#include <QTextCursor>

GraphView::GraphView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent) {
    // this->setMouseTracking(false);
    // this->viewport()->setMouseTracking(false);
    // this->setCursor(Qt::IBeamCursor);
    this->setRenderHint(QPainter::Antialiasing, true);
    // qDebug() << "graphview initialized!";
}

void GraphView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton){
        // QGraphicsItem* item = this->scene()->itemAt(event->scenePosition(), this->transform());
        // if (!item){
        emit mouse_clicked(event);
        // }
        // qDebug() << "graphview clicked!";
    }
    // this->setCursor(Qt::PointingHandCursor);
    // qDebug() << "view mouse pressed!";
    QGraphicsView::mousePressEvent(event);
}

void GraphView::mouseMoveEvent(QMouseEvent *event)
{
    // if (!is_hovering){
    // qDebug() << "mouse is moving!";
    // }
    if (current_tool == Tools::ERASER && event->buttons() == Qt::LeftButton){
        // qDebug() << "mouse left";
        QGraphicsItem* item = this->scene()->itemAt(event->scenePosition(), this->transform());
        if (item && !item_to_delete.contains(item)){
            // delete item;
            // qDebug() << item;
            item->setOpacity(0.5);
            item_to_delete.append(item);
        }
    }
    QGraphicsView::mouseMoveEvent(event);
}

void GraphView::mouseReleaseEvent(QMouseEvent *event)
{
    // qDebug() << "mouse button released";
    // this->setCursor(Qt::IBeamCursor);
    if (!item_to_delete.empty()){
        // for (auto&& item: item_to_delete) {
        //     delete item;
        // }
        for (int i = 0; i < item_to_delete.size(); i++) {
            // qDebug() << item_to_delete[i];
            delete item_to_delete[i];
        }
        item_to_delete.clear();
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void GraphView::text_tool_visible(bool status)
{
    if (status){
        // new_text = true;
        this->show();
        this->setInteractive(true);
        emit restore_pic();

    }else{
        // new_text = false;
        this->hide();
        this->setInteractive(false);
        this->scene()->clearFocus();
        this->scene()->update();
        const auto &items = this->scene()->items();
        for (const auto &item : items) {
            if (item->type() == TextBox::Type){
                auto textbox = static_cast<TextBox*>(item);
                QTextCursor cursor = textbox->textCursor();
                cursor.clearSelection();
                textbox->setTextCursor(cursor);
            }
        }
        // qDebug() << "clear focus";
    }
}

void GraphView::set_cursor(const QCursor &cursor)
{
    this->viewport()->setCursor(cursor);
}

void GraphView::set_tool(Tools tool)
{
    this->current_tool = tool;
}
