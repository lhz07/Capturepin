#ifndef PIN_H
#define PIN_H

#include "toolbar.h"
// #include "textbox.h"
#include "graphview.h"
#include <QWidget>
#include <QtWidgets>
#include <QLocalSocket>

namespace Ui {
class Pin;
}

class Pin : public QWidget
{
    Q_OBJECT

public:
    explicit Pin(QPixmap pic, QPoint p_start, QWidget *parent = nullptr);
    ~Pin();
    void hide_all();
    void show_all();
    // void text_tool_visible(bool status);
    // void select_textbox(TextBox* textbox);
    // void receive_response(QString response);
    // void set_add_new_component(bool status);
    ToolBar* toolbar;
protected:
    void mouseMoveEvent(QMouseEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void contextMenuEvent(QContextMenuEvent *) override;
    void wheelEvent(QWheelEvent *) override;
    // bool event(QEvent *event) override;
    void showEvent(QShowEvent* event) override;
    // void paintEvent(QPaintEvent *event) override;
private:
    Ui::Pin *ui;
    bool m_dragging = false;             // 标记是否正在拖动
    QPoint m_dragPosition;       // 鼠标拖动起始位置
    QVBoxLayout* mainLayout;
    QLabel* label_pic;
    bool get_id = true;
    bool is_hide = false;
    bool show_bar = false;
    bool to_show_bar = false;
    int* paint_count;
    QGraphicsScene* scene;
    GraphView* view;
    // QString windowId;
    QPoint p_start;
    QPoint start_move;
    QString window_title;
    QString pos;
    QPixmap pic;
    QProcess *process_mouse;
    QSize origin_size;
    QMenu* menu;
    QToolBar* text_adjust;
    QAction* quickSaveAction;
    QAction* saveAction;
    QAction* copyAction;
    QAction* closeAction;
    QAction* toolbarAction;
    QAction* addTextAction;
    QSettings myset = QSettings("CapturePin", "Config");
    HyprSocket* pin_socket;
    int up = 0;
    int down = 0;
    int scale_size = 10;
    void scale_pic(int scale_size);
    void copy();
    void addText();
    void show_toolbar();
    // void color_pick();
    void toggle_toolbar_visible();
    QShortcut* copy_pic;
    // QShortcut* add_text;
    QShortcut* key_show_bar;
    QAction* text_color;
    QLocalSocket* socket;
    // TextBox* selected_textbox = nullptr;
    void window_shown();
private slots:
    void save_pic();
    void quick_save();
    void receive_socket();
signals:
    // void process_key();
    void show_tool_bar();
    void close_process(QString window_title);
    void restore_toolbar();
};

#endif // PIN_H
