#ifndef TEXTBOX_H
#define TEXTBOX_H

#include <QObject>
#include <qgraphicsitem.h>
#include <QtWidgets>

class TextBox : public QGraphicsTextItem
{
    Q_OBJECT
public:
    explicit TextBox(QGraphicsItem* parent = nullptr);
    ~TextBox();
    void handle_external_mouse_event(QMouseEvent* event);
    enum control_area {NONE, CLOSE, RESIZE, ROTATE, EDIT, MOVE};

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void wheelEvent(QGraphicsSceneWheelEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
private:

    QVector<QPointF> handles;
    QTextCursor old_cursor;
    int up = 0;
    int down = 0;
    QMenu* context_menu;
    QAction* del_textbox;
    control_area current_control = NONE;
    void del_this();
    control_area detect_area(QPointF pos);
    QRectF close_area();
    QRectF resize_area();
    QRectF edit_area();
    QRectF rotate_area();
    QCursor rotate_cursor;
    QPointF rotate_start_pos;
    QPointF rotate_new_pos;
    double rotate_angle;
    QPointF resize_start_pos;
    QPointF resize_new_pos;
    QRectF resize_initial_rect;
    double resize_initial_text_size;

    bool is_hovering = false;
    bool is_adjusting = false;
    bool is_selected = false;

    void correct_center(QPointF center);

signals:
    // void select_textbox(QPointer<TextBox> textbox);
    void hovering_textbox(bool status);
    void update_font_size(double size);
    double get_font_size();
};

#endif // TEXTBOX_H
