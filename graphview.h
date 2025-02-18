#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include "textbox.h"
#include <QObject>
#include <QtWidgets>
class GraphView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphView(QGraphicsScene *scene, QWidget *parent = nullptr);
    void text_tool_visible(bool status);


protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
private:
    char current_tool;
    QList<TextBox*> textbox_list;
    void current_object(QObject* object);

signals:
    void mouse_clicked(QMouseEvent* event);
};

#endif // GRAPHVIEW_H
