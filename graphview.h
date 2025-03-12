#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include <QObject>
// #include <QtWidgets>
#include <QGraphicsView>
#include "sharevar.h"

class GraphView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphView(QGraphicsScene *scene, QWidget *parent = nullptr);
public slots:
    void text_tool_visible(bool status);
    void set_cursor(const QCursor& cursor);
    void set_tool(Tools tool);


protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
private:
    QList<QGraphicsItem*> item_to_delete;
    Tools current_tool = Tools::NONE;

signals:
    void mouse_clicked(QMouseEvent* event);
    void restore_pic();
};

#endif // GRAPHVIEW_H
