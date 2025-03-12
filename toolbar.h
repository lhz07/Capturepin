#ifndef TOOLBAR_H
#define TOOLBAR_H

#include "graphview.h"
#include "hyprsocket.h"
#include "sharevar.h"
// #include "textbox.h"
#include <QLocalSocket>
#include <QWidget>
#include <QToolBar>
#include <QPointer>
#include <QSettings>
#include <QShortcut>
#include <QFontComboBox>
#include <QPushButton>
#include "pensizebutton.h"
// #include <QtWidgets>

// namespace Ui {
// class ToolBar;
// }

class ToolBar : public QWidget
{
    Q_OBJECT

public:
    explicit ToolBar(QGraphicsScene* scene, QString window_title, QWidget *parent = nullptr);
    ~ToolBar();
public slots:
    void show_toolbar();
    void restore_toolbar();
    void get_focus_item(QGraphicsItem *newFocusItem, QGraphicsItem *oldFocusItem, Qt::FocusReason reason);
    void get_selected_color(const QColor &color);
    void screen_clicked(QMouseEvent* event);
    void update_font_size(double size);
    void set_color_cursor(const QColor& color, int size, bool bound = false);


protected:
    // bool event(QEvent *event) override;
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private:
    // Ui::ToolBar *ui;
    QToolBar* toolbar;
    QPointer<QToolBar> text_adjust;
    QPointer<QToolBar> line_tool;
    QPointer<QToolBar> eraser_tool;
    QWidget* parent;
    QAction* addText;
    QAction* addLine;
    QAction* eraser;
    QAction* complete;
    // QAction* text_color;
    QAction* bold_text;
    QAction* italic_text;
    QPushButton* text_color_button;
    QPushButton* line_color_button;
    bool to_show_bar = false;
    bool reshow_bar = false;
    Tools current_tool;
    const int default_font_size;
    QSettings myset = QSettings("CapturePin", "Config");

    // QPointer<TextBox> selected_textbox = nullptr;
    QString window_title;
    QColor line_color;
    QColor textbox_color;
    QGraphicsScene* scene;
    GraphView* view;
    QShortcut* key_toggle_bar;
    QFontComboBox* text_font;
    QComboBox* text_size;
    HyprSocket* toolbar_socket;
    QLocalSocket* socket;
    PenSizeButton* line_width_button;
    PenSizeButton* eraser_width_button;
    void window_shown();
    void update_item_color(const QColor& color);
    void reset_last_tool();
private slots:
    void receive_socket();
    void addText_button_clicked(bool checked);
    void addLine_button_clicked(bool checked);
    void eraser_button_clicked(bool checked);
    void bold_text_button_clicked(bool checked);
    void italic_text_button_clicked(bool checked);
    void color_dialog();
    void change_text_size(const QString& size);
    void change_text_font(const QFont& font);
    void edit_done();
    void delete_focus_item();

signals:
    void view_visible(bool status);
    void toggle_toolbar_visible();
    void update_pic(const QPixmap& new_pic);
    void set_view_cursor(const QCursor& cursor);
    void set_view_tool(Tools tool);
};

#endif // TOOLBAR_H
