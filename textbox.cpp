#include "textbox.h"
#include <qgraphicssceneevent.h>
#include <qpainter.h>

TextBox::TextBox(QGraphicsItem* parent)
    : QGraphicsTextItem(parent)
{
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable);
    setTextInteractionFlags(Qt::TextEditorInteraction);
    this->setAcceptHoverEvents(true);
    old_cursor = this->textCursor();
    del_textbox = new QAction("Close textbox");
    context_menu = new QMenu();
    context_menu->addAction(del_textbox);
    connect(del_textbox, &QAction::triggered, this, &TextBox::del_this);
    rotate_cursor = QCursor(QPixmap(":/pic/resource/pic/rotate_cursor.svg").scaledToHeight(20, Qt::SmoothTransformation));
    close_button = new GraphCloseButton(this);
    connect(close_button, &GraphCloseButton::clicked, this, &TextBox::close_button_clicked);
    // this->setTransformOriginPoint(this->boundingRect().center());
    update();

}

TextBox::~TextBox()
{
    // qDebug() << "textbox delete!";
    this->scene()->update();
}

void TextBox::close_button_clicked()
{
    del_this();
}

void TextBox::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsTextItem::hoverMoveEvent(event);
    current_control = detect_area(event->pos());
    switch (current_control) {
    case ROTATE:{
        this->scene()->views()[0]->viewport()->setCursor(rotate_cursor);
        this->is_hovering = true;
        this->scene()->update();
        // qDebug() << "rotate area" << this->toPlainText();
        break;
    }
    case RESIZE:
        this->setCursor(Qt::SizeFDiagCursor);
        this->scene()->views()[0]->viewport()->setCursor(Qt::SizeFDiagCursor);
        this->is_hovering = true;
        this->scene()->update();
        // qDebug() << "resize area" << this->toPlainText();
        break;
    case MOVE:
        this->setCursor(Qt::SizeAllCursor);
        // this->scene()->views()[0]->viewport()->setCursor(Qt::IBeamCursor);
        this->is_hovering = true;
        this->scene()->update();
        // qDebug() << "move area" << this->toPlainText();
        break;
    case EDIT:
        this->setCursor(Qt::IBeamCursor);
        // this->scene()->views()[0]->viewport()->setCursor(Qt::IBeamCursor);
        this->is_hovering = true;
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
        break;
    }
    // emit hovering_textbox(true);
    // qDebug() << "hover move" << this->toPlainText();
}

void TextBox::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    // qDebug() << "hover enter" << this->toPlainText();
    QGraphicsTextItem::hoverEnterEvent(event);
    // QTextCursor cursor;
    // this->setTextCursor(cursor);
    // this->setFocus(); // 鼠标进入时设置焦点，显示虚线框
}

void TextBox::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    // this->scene()->views()[0]->viewport()->setCursor(Qt::IBeamCursor);
    // qDebug() << "hover leave";
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
    this->scene()->update();
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
    // correct_center();
    this->scene()->update();
    QGraphicsTextItem::keyPressEvent(event);
}

void TextBox::del_this()
{
    this->deleteLater();
    // this->setCursor(Qt::ArrowCursor);
}

QRectF TextBox::resize_area()
{
    QPointF bottom_right = this->boundingRect().bottomRight();
    return QRectF(bottom_right.x() - 4, bottom_right.y() - 4, 5, 5);
}

QRectF TextBox::edit_area()
{
    return QRectF(4, 4, this->boundingRect().width() - 8, this->boundingRect().height() - 8);
}

QRectF TextBox::rotate_area()
{
    return QRectF(this->boundingRect().center().x() - 4, this->boundingRect().top() - 10, 8, 8);
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
    if (rotate_area().contains(pos)){
        return ROTATE;
    }else if(resize_area().contains(pos)){
        return RESIZE;
    }else if (this->boundingRect().contains(pos) && !edit_area().contains(pos)){
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
    QGraphicsTextItem::paint(painter, option, widget);
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
    if (current_control){
        // painter->drawImage(close_area(), QImage(":/pic/resource/pic/close.svg"));
        close_button->setPos(this->boundingRect().topRight());
        painter->drawImage(rotate_area(), QImage(":/pic/resource/pic/rotate.svg"));
        QPen pen;
        pen.setColor(Qt::black);
        pen.setWidth(1);
        painter->setPen(pen);
        painter->setBrush(Qt::white);
        // painter->setBrush(Qt::white);
        // painter->drawImage(resize_area(), QImage(":/pic/resource/pic/resize.svg"));
        painter->drawRoundedRect(resize_area(), 3, 3);
    }
}

void TextBox::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    // this->setPos(event->pos());
    // event->ignore();
    switch (current_control) {
    case ROTATE:
        correct_center(this->boundingRect().center());
        rotate_angle = this->rotation();
        rotate_start_pos = event->scenePos();
        is_adjusting = true;
        this->update();
        this->scene()->update();
        break;
    case RESIZE:
        correct_center(this->boundingRect().topLeft());
        resize_initial_rect = this->boundingRect();
        resize_initial_text_size = this->font().pointSizeF();
        // qDebug() << event->pos();
        resize_start_pos = event->pos();
        is_adjusting = true;
        this->update();
        this->scene()->update();
        break;
    case EDIT:
        break;
    case MOVE:
        break;
    case NONE:
        this->is_selected = false;
        break;
        // this->setFocus();
    }
    if (current_control != RESIZE){
        QGraphicsTextItem::mousePressEvent(event);
    }
    // qDebug() << "mouse pressed123";
    // qDebug() << event->pos();
}

void TextBox::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // this->update();
    // this->scene()->update();
    switch (current_control) {
    case ROTATE:{
        rotate_new_pos = event->scenePos();
        QPointF center_pos = this->mapToScene(this->boundingRect().center());
        QLineF line1(center_pos, rotate_start_pos);
        QLineF line2(center_pos, rotate_new_pos);
        double add_angle = line2.angleTo(line1);
        this->setRotation(rotate_angle + add_angle);
        rotate_angle  = this->rotation();
        rotate_start_pos = rotate_new_pos;
        break;
    }
    case RESIZE:{
        resize_new_pos = event->pos();

        QPointF delta = event->pos() - resize_start_pos;
        // resize_start_pos = resize_new_pos;
        qreal newWidth = resize_initial_rect.width() + delta.x();
        qreal newHeight = resize_initial_rect.height() + delta.y();

        // 计算缩放比例
        qreal scaleX = newWidth / resize_initial_rect.width();
        qreal scaleY = newHeight / resize_initial_rect.height();
        qreal scale = (scaleX + scaleY) / 2.0;
        this->setScale(this->scale() * scale);
        emit update_font_size(qRound(this->font().pointSizeF() * 10 * this->scale()) / 10.0);
        // QFont newFont = this->font();
        // newFont.setPointSizeF(qRound(resize_initial_text_size * scale * 10) / 10.0);
        // this->setFont(newFont);
        // qDebug() << scale << newFont.pointSizeF();
        break;
    }
    case NONE:
    case EDIT:
    case MOVE:
        QGraphicsTextItem::mouseMoveEvent(event);
        break;
    }
    this->update();
    this->scene()->update();
}

void TextBox::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    switch (current_control) {
    case RESIZE:{
        // double size = emit get_font_size();
        // QFont new_font = this->font();
        // new_font.setPointSizeF(size / 2.0);
        // this->setFont(new_font);
        // this->setScale(2);
        this->update();
        this->scene()->update();
        break;
    }
    default:
        break;
    }
    QGraphicsTextItem::mouseReleaseEvent(event);
}
