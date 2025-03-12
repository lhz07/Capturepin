#ifndef LLINE_H
#define LLINE_H

#include <QGraphicsLineItem>
#include <QMenu>
#include <QAction>
#include <QObject>
#include "lcircle.h"

class Lline : public QObject, public QGraphicsLineItem
{
    Q_OBJECT
public:
    explicit Lline(QObject* parent = nullptr);
    ~Lline();
    enum control_area {NONE, L_DRAG, R_DRAG, MOVE};
protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    QPainterPath shape() const override;
private:
    LCircle right_handle() const;
    LCircle left_handle() const;
    control_area detect_area(const QPointF &point);
    control_area current_control;
    QMenu* context_menu;
    QAction* del;
    void del_this();
    QPointF start_pos;
    bool grade_rotation;
    bool first_draw = true;

};

#endif // LLINE_H
