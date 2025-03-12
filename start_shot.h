#ifndef START_SHOT_H
#define START_SHOT_H

// #include <QMainWindow>
#include <QOpenGLWidget>
// #include <QtWidgets>
#include "pin.h"
#include "hyprsocket.h"

// namespace Ui {
// class Start_shot;
// }

class Start_shot : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit Start_shot(QWidget *parent = nullptr);
    ~Start_shot();
    static int pin_count;
    void pin_picture();
    Pin* pin1;
protected:
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void showEvent(QShowEvent* event) override;
    void keyPressEvent(QKeyEvent *) override;
    void keyReleaseEvent(QKeyEvent *) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
private:
    // Ui::Start_shot *ui;
    // QLabel *label_pic;
    QShortcut *pin_pic;
    QPixmap *res;
    double pixel_ratio;
    QPoint p_start;
    QPoint p_end;
    QPoint cursor_pos;
    QPoint rd;
    QPoint pin_pic_pos;
    QShortcut* copy_pic;
    QShortcut* up;
    QShortcut* cancel;
    QLocalSocket* socket;
    HyprSocket* shot_socket;
    QString current_monitor;
    QString current_workspace_id;
    QScreen* current_screen;
    QMenu* menu;
    int show_count = 0;
    void move_up();
    void copy();
    void save_pic();
    void auto_save(QPixmap pic);
    bool draw_completed = false;
    bool start_move = false;
    bool right_move = false;
    bool left_move = false;
    bool down_move = false;
    bool up_move = false;
    void correct_where_to_start(QPoint &p_start, QPoint &p_end);
    void window_shown();
private slots:
    void receive_socket();
signals:
    void start_process(QString window_title);
};

#endif // START_SHOT_H
