#ifndef TOOLBAR_H
#define TOOLBAR_H

#include "graphview.h"
#include "hyprsocket.h"
#include "textbox.h"
#include <QWidget>
#include <QtWidgets>

namespace Ui {
class ToolBar;
}

class ToolBar : public QWidget
{
    Q_OBJECT

public:
    explicit ToolBar(QGraphicsScene* scene, QString window_title, QWidget *parent = nullptr);
    ~ToolBar();
    void show_toolbar();
    void restore_toolbar();
    void get_focus_item(QGraphicsItem *newFocusItem, QGraphicsItem *oldFocusItem, Qt::FocusReason reason);
    void get_selected_color(const QColor &color);
    void screen_clicked(QMouseEvent* event);
    QToolBar* toolbar;
    QPointer<QToolBar> text_adjust;
    void update_font_size(double size);
    double get_font_size();

protected:
    bool event(QEvent *event) override;
    void closeEvent(QCloseEvent* event) override;

private:
    Ui::ToolBar *ui;
    QAction* addText;
    QAction* complete;
    QAction* text_color;
    int paint_count = 0;
    bool to_show_bar = false;
    void addText_button_clicked();
    QSettings myset = QSettings("CapturePin", "Config");
    void change_text_color();
    // QPointer<TextBox> selected_textbox = nullptr;
    QString window_title;
    QColor selected_color;
    unsigned char current_tool;
    QGraphicsScene* scene;
    GraphView* view;
    void edit_done();
    QShortcut* key_toggle_bar;
    QFontComboBox* text_font;
    QComboBox* text_size;
    HyprSocket* toolbar_socket;
    bool reshow_bar = false;

    void change_text_size(const QString& size);
    void change_text_font(const QFont& font);


signals:
    void view_visible(bool status);
    void toggle_toolbar_visible();
    // void send_current_tool(char tool);
};

#endif // TOOLBAR_H
