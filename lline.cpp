#include "lline.h"
#include <QPainter>
#include <QGraphicsSceneEvent>
#include <QCursor>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QKeyEvent>


Lline::Lline(QObject *parent) : QObject(parent)
{
    this->setAcceptHoverEvents(true);
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable);
    context_menu = new QMenu();
    del = new QAction("delete", this);
    context_menu->addAction(del);
    connect(del, &QAction::triggered, this, &Lline::del_this);
}

Lline::~Lline()
{
    this->scene()->views().constFirst()->blockSignals(false);
}

void Lline::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsLineItem::paint(painter, option, widget);
    painter->setRenderHint(QPainter::Antialiasing);
    if (this->hasFocus() && !first_draw){
        painter->setPen(Qt::NoPen);
        painter->save();
        painter->setBrush(QBrush(Qt::white));
        // painter->drawEllipse(right_handle());
        // painter->drawEllipse(left_handle());
        painter->drawEllipse(this->left_handle().center(), this->left_handle().radius(), this->left_handle().radius());
        painter->drawEllipse(this->right_handle().center(), this->right_handle().radius(), this->right_handle().radius());
        painter->restore();
        QPen pen;
        pen.setColor(Qt::white);
        pen.setWidth(1);
        painter->setPen(pen);
        painter->setCompositionMode(QPainter::CompositionMode_Difference);
        // painter->drawEllipse(right_handle());
        // painter->drawEllipse(left_handle());
        painter->drawEllipse(this->left_handle().center(), this->left_handle().radius(), this->left_handle().radius());
        painter->drawEllipse(this->right_handle().center(), this->right_handle().radius(), this->right_handle().radius());
    }
}

void Lline::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsLineItem::mousePressEvent(event);
    current_control = detect_area(event->pos());
    if (first_draw){
        QPixmap cursorPixmap(8, 8);
        cursorPixmap.fill(Qt::transparent);

        // QPainter painter(&cursorPixmap);
        // painter.setRenderHint(QPainter::Antialiasing);
        // painter.setPen(Qt::NoPen);
        // painter.setBrush(QBrush(Qt::blue));
        // painter.drawEllipse(QRectF(0, 0, 8, 8));
        current_control = R_DRAG;
        this->setCursor(QCursor(cursorPixmap, 5, 5));
    }
    qDebug() << current_control;
    switch (current_control) {
    case L_DRAG:
        start_pos = this->mapToScene(this->line().p2());
        break;
    case R_DRAG:
        start_pos = this->mapToScene(this->line().p1());
    default:
        break;
    }
}

void Lline::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsLineItem::mouseMoveEvent(event);
    switch (current_control) {
    case L_DRAG:
    case R_DRAG:
    {
        QPointF new_pos = event->pos();
        // qDebug() << this->mapFromScene(start_pos) << new_pos;
        qDebug() << this->mapToScene(this->line().p1()) << this->mapToScene(this->line().p2());
        this->setLine(QLineF(this->mapFromScene(start_pos), new_pos));
        break;
    }
    default:
        break;
    }
    this->update();
    this->scene()->update();
}

void Lline::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsLineItem::mouseReleaseEvent(event);
    if (first_draw){
        first_draw = false;
    }
    // qDebug() << "release";
}

void Lline::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsLineItem::hoverMoveEvent(event);
    // qDebug() << "hover move";
    switch (detect_area(event->pos())) {
    case NONE:
        this->setCursor(Qt::ArrowCursor);
        break;
    case L_DRAG:
    case R_DRAG:
        this->setCursor(Qt::SizeFDiagCursor);
        break;
    case MOVE:
        this->setCursor(Qt::SizeAllCursor);
        break;
    default:
        break;
    }
    this->update();
    this->scene()->update();
}

void Lline::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    this->scene()->views().constFirst()->blockSignals(true);
    // qDebug() << "hover enter";
}

void Lline::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    this->scene()->views().constFirst()->blockSignals(false);
}

void Lline::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    context_menu->exec(event->screenPos());
}

void Lline::keyPressEvent(QKeyEvent *event)
{
    QGraphicsLineItem::keyPressEvent(event);
    if (event->key() == Qt::Key_Shift){
        grade_rotation = true;
    }
}

void Lline::keyReleaseEvent(QKeyEvent *event)
{
    QGraphicsLineItem::keyReleaseEvent(event);
    if (event->key() == Qt::Key_Shift){
        grade_rotation = false;
    }
}

QPainterPath Lline::shape() const
{
    auto path = QGraphicsLineItem::shape();
    path.addEllipse(this->left_handle().center(), this->left_handle().radius(), this->left_handle().radius());
    path.addEllipse(this->right_handle().center(), this->right_handle().radius(), this->right_handle().radius());
    return path;
}

LCircle Lline::right_handle() const
{
    // if (this->line().p1().x() <= this->line().p2().x() && this->line().p1().y() >= this->line().p2().y()){
    //     return QRectF(this->boundingRect().topRight() - QPointF(6, 0), QSizeF(6, 6));
    // }else if (this->line().p1().x() > this->line().p2().x() && this->line().p1().y() < this->line().p2().y()){
    //     return QRectF(this->boundingRect().bottomLeft() - QPointF(0, 6), QSizeF(6, 6));
    // }else {
    //     return QRectF(this->boundingRect().bottomRight() - QPointF(6, 6), QSizeF(6, 6));
    // }
    return LCircle(this->line().p2(), 3);
}

LCircle Lline::left_handle() const
{
    // if (this->line().p1().x() <= this->line().p2().x() && this->line().p1().y() >= this->line().p2().y()){
    //     return QRectF(this->boundingRect().bottomLeft() - QPointF(0, 6), QSizeF(6, 6));
    // }else if (this->line().p1().x() > this->line().p2().x() && this->line().p1().y() < this->line().p2().y()){
    //     return QRectF(this->boundingRect().topRight() - QPointF(6, 0), QSizeF(6, 6));
    // }else {
    //     return QRectF(this->boundingRect().topLeft(), QSizeF(6, 6));
    // }
    return LCircle(this->line().p1(), 3);
}

void Lline::del_this()
{
    this->deleteLater();
}

Lline::control_area Lline::detect_area(const QPointF &point)
{
    if (left_handle().contains(point)){
        return L_DRAG;
    }else if (right_handle().contains(point)){
        return R_DRAG;
    }else if (this->boundingRect().contains(point)){
        return MOVE;
    }else {
        return NONE;
    }
}
