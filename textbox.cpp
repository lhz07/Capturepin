#include "textbox.h"
#include "sharevar.h"
#include <qevent.h>
#include <qgraphicsscene.h>
#include <qgraphicssceneevent.h>
#include <qgraphicsview.h>
#include <qmenu.h>
#include <qpainter.h>
#include <qstyleoption.h>

TextBox::TextBox(QGraphicsItem* parent)
    : QGraphicsTextItem(parent), default_font_size(ShareVar::default_font_size)
{
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable);
    setTextInteractionFlags(Qt::TextEditorInteraction);
    this->setAcceptHoverEvents(true);
    del_textbox = new QAction("Close textbox");
    context_menu = new QMenu();
    context_menu->addAction(del_textbox);
    connect(del_textbox, &QAction::triggered, this, &TextBox::del_this);
    close_button = new GraphCloseButton(this);
    close_button->setVisible(false);
    close_button->setZValue(2);
    connect(close_button, &GraphCloseButton::clicked, this, &TextBox::close_button_clicked);
    resize_button = new GraphResizeButton(this);
    resize_button->setVisible(false);
    resize_button->setZValue(2);
    connect(resize_button, &GraphResizeButton::mouse_press, this, &TextBox::resize_button_press);
    connect(resize_button, &GraphResizeButton::mouse_move, this, &TextBox::resize_button_move);
    // connect(resize_button, &GraphResizeButton::mouse_release, this, &TextBox::resize_button_release);
    rotate_button = new GraphRotateButton(this);
    rotate_button->setVisible(false);
    rotate_button->setZValue(2);
    connect(rotate_button, &GraphRotateButton::mouse_press, this, &TextBox::rotate_button_press);
    connect(rotate_button, &GraphRotateButton::mouse_move, this, &TextBox::rotate_button_move);
    old_bound_rect = this->boundingRect();
    close_button->setPos(this->boundingRect().topRight());
    resize_button->setPos(this->boundingRect().bottomRight());
    rotate_button->setPos(this->boundingRect().center().x(), this->boundingRect().top());
    // this->setTransformOriginPoint(this->boundingRect().center());
}

TextBox::~TextBox()
{
    // qDebug() << this->toPlainText() << this << "textbox delete!";
    this->scene()->update();
}

void TextBox::close_button_clicked()
{
    del_this();
}

void TextBox::resize_button_press(QGraphicsSceneMouseEvent *event)
{
    this->setFocus();
    correct_center(this->boundingRect().topLeft());
    // resize_start_pos = this->mapFromItem(resize_button, event->pos());
    resize_start_pos = this->mapFromScene(event->scenePos());
    // qDebug() << resize_start_pos;
    this->update();
    this->scene()->update();
}

void TextBox::resize_button_move(QGraphicsSceneMouseEvent *event)
{

    // since we set flag "ItemIgnoresTransformations", there is a bug of mapfromitem
    // so we use mapfromscene
    // qDebug() << "from item:" << resize_new_pos;
    // qDebug() << "from scene:" << this->mapFromScene(event->scenePos());
    // resize_new_pos = this->mapFromItem(resize_button, event->pos());
    resize_new_pos = this->mapFromScene(event->scenePos());

    QPointF delta = resize_new_pos - resize_start_pos;
    // qDebug() << "resize button" << resize_start_pos << "new" << resize_new_pos;
    // qDebug() << "delta:" << delta;
    // resize_start_pos = resize_new_pos;

    // notice: boudingRect will never change when resize the textbox
    qreal newWidth = this->boundingRect().width() + delta.x();
    qreal newHeight = this->boundingRect().height() + delta.y();
    // qDebug() << "boundingrect:" << this->boundingRect();

    // calculate scale rate
    qreal scaleX = newWidth / this->boundingRect().width();
    qreal scaleY = newHeight / this->boundingRect().height();
    // a not accurate estimation
    qreal scale = (scaleX + scaleY) / 2.0;
    // qreal scale = qMax(scaleX, scaleY);
    // qDebug() << scale;
    this->setScale(this->scale() * scale);
    emit update_font_size(qRound(default_font_size * 10 * this->scale()) / 10.0);
    this->update();
    this->scene()->update();
}

void TextBox::rotate_button_press(QGraphicsSceneMouseEvent *event)
{
    this->setFocus();
    correct_center(this->boundingRect().center());
    rotate_angle = this->rotation();
    rotate_start_pos = event->scenePos();
    this->update();
    this->scene()->update();
}

void TextBox::rotate_button_move(QGraphicsSceneMouseEvent *event)
{
    rotate_new_pos = event->scenePos();
    QPointF center_pos = this->mapToScene(this->boundingRect().center());
    QLineF line1(center_pos, rotate_start_pos);
    QLineF line2(center_pos, rotate_new_pos);
    double add_angle = line2.angleTo(line1);
    double new_angle = rotate_angle + add_angle;
    int grade_angle = 0;
    if (new_angle > 360){
        new_angle = new_angle - 360;
    }
    // qDebug() << new_angle;
    if (new_angle < 22.5 || new_angle >= 337.5){
        resize_button->setCursor(Qt::SizeFDiagCursor);
        grade_angle = 0;
    }else if (new_angle < 67.5){
        resize_button->setCursor(Qt::SizeVerCursor);
        grade_angle = 45;
    }else if (new_angle < 112.5){
        resize_button->setCursor(Qt::SizeBDiagCursor);
        grade_angle = 90;
    }else if (new_angle < 157.5){
        resize_button->setCursor(Qt::SizeHorCursor);
        grade_angle = 135;
    }else if (new_angle < 202.5){
        resize_button->setCursor(Qt::SizeFDiagCursor);
        grade_angle = 180;
    }else if (new_angle < 247.5){
        resize_button->setCursor(Qt::SizeVerCursor);
        grade_angle = 225;
    }else if (new_angle < 292.5){
        resize_button->setCursor(Qt::SizeBDiagCursor);
        grade_angle = 270;
    }else if (new_angle < 337.5){
        resize_button->setCursor(Qt::SizeHorCursor);
        grade_angle = 315;
    }
    if (grade_rotation){
        this->setRotation(grade_angle);
    }else{
        this->setRotation(new_angle);
    }
    // no need to update them here, just compare the new pos with the original pos
    // rotate_angle  = this->rotation();
    // rotate_start_pos = rotate_new_pos;
    close_button->setRotation(this->rotation());
    resize_button->setRotation(this->rotation());
    rotate_button->setRotation(this->rotation());
    this->update();
    this->scene()->update();

}

void TextBox::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsTextItem::hoverMoveEvent(event);
    current_control = detect_area(event->pos());
    switch (current_control) {
    case MOVE:
        this->setCursor(Qt::SizeAllCursor);
        // this->scene()->views()[0]->viewport()->setCursor(Qt::IBeamCursor);
        // this->is_hovering = true;
        this->scene()->update();
        // qDebug() << "move area" << this->toPlainText();
        break;
    case EDIT:
        this->setCursor(Qt::IBeamCursor);
        // this->scene()->views()[0]->viewport()->setCursor(Qt::IBeamCursor);
        // this->is_hovering = true;
        this->scene()->update();
        // qDebug() << "edit_area" << this->toPlainText();
        break;
    case NONE:
        // if (this->is_hovering){
        //     this->setCursor(Qt::IBeamCursor);
        //     this->scene()->views()[0]->viewport()->setCursor(Qt::IBeamCursor);
        //     emit hovering_textbox(false);
        //     this->is_hovering = false;
        //     this->scene()->update();
        // qDebug() << "other area" << this->toPlainText();
        // }
        // notice: even when the cursor is not in the boundingRect, it may still belongs to TextBox
        // for about 1-2 pixels
        this->setCursor(Qt::IBeamCursor);
        break;
    }
    // emit hovering_textbox(true);
    // qDebug() << "hover move" << this->toPlainText();
}

void TextBox::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    // qDebug() << "hover enter" << this->toPlainText();
    this->is_hovering = true;
    this->update();
    QGraphicsTextItem::hoverEnterEvent(event);
    // QTextCursor cursor;
    // this->setTextCursor(cursor);
    // this->setFocus(); // 鼠标进入时设置焦点，显示虚线框
}

void TextBox::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    // this->scene()->views()[0]->viewport()->setCursor(Qt::IBeamCursor);
    // qDebug() << "hover leave" << this->toPlainText();
    this->is_hovering = false;
    this->update();
    QGraphicsTextItem::hoverLeaveEvent(event);
    // this->clearFocus(); // 鼠标离开时清除焦点，隐藏虚线框
}

void TextBox::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    int temp = event->delta();
    double scale_rate = temp / 16.0;
    double font_size = this->font().pointSizeF() + scale_rate;
    if (font_size <= 0.5){
        font_size = 0.5;
    }else if (font_size > 500){
        font_size = 500;
    }
    QFont temp_font = QFont(this->font());
    temp_font.setPointSizeF(font_size);
    this->setFont(temp_font);
    this->setTransformOriginPoint(this->boundingRect().center());
    // this->scene()->update();
    // qDebug() << this->font().pointSizeF();
}

void TextBox::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{


    // 获取原始右键菜单
    // QGraphicsTextItem::contextMenuEvent(event);  // 保留原有菜单

    // 将原菜单添加到新的菜单中
    // contextMenu.addAction(defaultAction);

    // 添加新的自定义菜单项
    // QAction* newAction = contextMenu.addAction("New Custom Action");
    // connect(newAction, &QAction::triggered, this, &CustomGraphicsTextItem::onNewActionTriggered);

    // 显示菜单
    context_menu->exec(event->screenPos());
}

void TextBox::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Shift){
        grade_rotation = true;
    }
    QGraphicsTextItem::keyPressEvent(event);
}

void TextBox::keyReleaseEvent(QKeyEvent *event)
{
    this->scene()->update();
    if (event->key() == Qt::Key_Shift){
        grade_rotation = false;
    }
    QGraphicsTextItem::keyReleaseEvent(event);
}

void TextBox::focusInEvent(QFocusEvent *event)
{
    close_button->setVisible(true);
    resize_button->setVisible(true);
    rotate_button->setVisible(true);
    can_delete = false;
    // qDebug() << this << "can delete:" << can_delete;
    // qDebug() << "focus in";
    QGraphicsTextItem::focusInEvent(event);
}

void TextBox::focusOutEvent(QFocusEvent *event)
{
    // qDebug() << "focus out";
    QGraphicsTextItem::focusOutEvent(event);
    if (close_button->hide() && resize_button->hide() && rotate_button->hide()){
        if (this->toPlainText().isEmpty()){
            can_delete = true;
            // qDebug() << this << "can delete:" << can_delete;
        }
    }
}

void TextBox::del_this()
{
    this->deleteLater();
    // this->setCursor(Qt::ArrowCursor);
}

QRectF TextBox::edit_area()
{
    return QRectF(4, 4, this->boundingRect().width() - 8, this->boundingRect().height() - 8);
}

void TextBox::correct_center(QPointF center)
{
    // 记录原来的场景中心
    QPointF oldSceneCenter = mapToScene(boundingRect().center());

    // 更新变换原点
    this->setTransformOriginPoint(center);

    // 记录新的场景中心
    QPointF newSceneCenter = mapToScene(boundingRect().center());

    // 调整位置以保持中心不变
    this->setPos(pos() + (oldSceneCenter - newSceneCenter));
}

TextBox::control_area TextBox::detect_area(QPointF pos)
{
    if (this->boundingRect().contains(pos) && !edit_area().contains(pos)){
        return MOVE;
        // qDebug() << "move area" << this->toPlainText();
    }else if (edit_area().contains(pos)){
        return EDIT;
        // qDebug() << "edit_area" << this->toPlainText();
    }
    return NONE;
}

void TextBox::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    QStyleOptionGraphicsItem new_option(*option);

    if (new_option.state & QStyle::State_Selected){
        new_option.state &= ~QStyle::State_Selected;
    }
    if (new_option.state & QStyle::State_HasFocus){
        new_option.state &= ~QStyle::State_HasFocus;
    }
    QGraphicsTextItem::paint(painter, &new_option, widget);
    // QRectF rect(0, 0, 200, 100);
    // // 绘制边框和控制点
    // painter->setPen(QPen(Qt::blue, 1, Qt::DashLine));
    // painter->drawRect(rect);

    // // 控制点
    // painter->setBrush(Qt::blue);
    // for (const auto& handle : handles) {
    //     painter->drawEllipse(handle, 5, 5);
    // }

    //close button
    // painter->setPen(Qt::black);


    // draw bound
    if (this->is_hovering || this->hasFocus()){
        QPen pen_bound;
        if (this->hasFocus()){
            pen_bound.setStyle(Qt::SolidLine);
            painter->save();
            // painter->drawImage(close_area(), QImage(":/pic/resource/pic/close.svg"));
            if (this->boundingRect() != old_bound_rect){
                old_bound_rect = this->boundingRect();
                close_button->setPos(this->boundingRect().topRight());
                resize_button->setPos(this->boundingRect().bottomRight());
                rotate_button->setPos(this->boundingRect().center().x(), this->boundingRect().top());
            }
            // QPen pen;
            // pen.setColor(Qt::black);
            // pen.setWidth(1);
            // painter->setPen(pen);
            // painter->setBrush(Qt::white);
            // painter->setBrush(Qt::white);
            // painter->drawImage(resize_area(), QImage(":/pic/resource/pic/resize.svg"));
            // painter->drawRoundedRect(resize_area(), 3, 3);
            painter->restore();
        }else if (this->is_hovering){
            pen_bound.setStyle(Qt::DashLine);
        }
        painter->save();
        pen_bound.setColor(Qt::white);
        pen_bound.setWidth(0);
        painter->setPen(pen_bound);
        painter->setCompositionMode(QPainter::CompositionMode_Difference);
        painter->drawRect(this->boundingRect());
        painter->restore();
    }
}

// void TextBox::mousePressEvent(QGraphicsSceneMouseEvent* event)
// {
//     QGraphicsTextItem::mousePressEvent(event);
// }

void TextBox::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // this->update();
    // this->scene()->update();
    switch (current_control) {
    case NONE:
    case EDIT:
    case MOVE:
        close_button->setPos(this->boundingRect().topRight());
        resize_button->setPos(this->boundingRect().bottomRight());
        rotate_button->setPos(this->boundingRect().center().x(), this->boundingRect().top());
        QGraphicsTextItem::mouseMoveEvent(event);
        break;
    }
    this->update();
    this->scene()->update();
}

// QVariant TextBox::itemChange(GraphicsItemChange change, const QVariant &value)
// {
//     qDebug() << change;
//     if (change == QGraphicsItem::ItemPositionChange){
//         close_button->setPos(this->boundingRect().topRight());
//         resize_button->setPos(this->boundingRect().bottomRight());
//         rotate_button->setPos(this->boundingRect().center().x(), this->boundingRect().top());
//     }
//     return QGraphicsTextItem::itemChange(change, value);
// }

// void TextBox::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
// {
//     QGraphicsTextItem::mouseReleaseEvent(event);
// }
