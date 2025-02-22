#ifndef TEXTBOX_H
#define TEXTBOX_H

#include <QObject>
#include <qaction.h>
#include <qgraphicsitem.h>
// #include <QtWidgets>
#include "graphclosebutton.h"
#include "graphresizebutton.h"
#include "graphrotatebutton.h"

class TextBox : public QGraphicsTextItem
{
    Q_OBJECT
public:
    explicit TextBox(QGraphicsItem* parent = nullptr);
    ~TextBox();
    enum control_area {NONE, EDIT, MOVE};
    void close_button_clicked();
    void resize_button_press(QGraphicsSceneMouseEvent* event);
    void resize_button_move(QGraphicsSceneMouseEvent* event);
    void rotate_button_press(QGraphicsSceneMouseEvent* event);
    void rotate_button_move(QGraphicsSceneMouseEvent* event);
    // void resize_button_release(QGraphicsSceneMouseEvent *event);

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    // void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    // void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void wheelEvent(QGraphicsSceneWheelEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
private:

    QVector<QPointF> handles;
    GraphCloseButton* close_button;
    GraphResizeButton* resize_button;
    GraphRotateButton* rotate_button;
    int up = 0;
    int down = 0;
    QMenu* context_menu;
    QAction* del_textbox;
    control_area current_control = NONE;
    void del_this();
    control_area detect_area(QPointF pos);
    QRectF edit_area();
    QPointF rotate_start_pos;
    QPointF rotate_new_pos;
    double rotate_angle;
    QPointF resize_start_pos;
    QPointF resize_new_pos;
    double resize_initial_text_size;

    bool is_hovering = false;
    bool grade_rotation = false;

    void correct_center(QPointF center);

signals:
    // void select_textbox(QPointer<TextBox> textbox);
    void update_font_size(double size);
    double get_font_size();
};

#endif // TEXTBOX_H
