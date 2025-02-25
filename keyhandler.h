#ifndef KEYHANDLER_H
#define KEYHANDLER_H

#include <QObject>
// #include <QtWidgets>

class KeyHandler : public QObject
{
    Q_OBJECT
public:
    static KeyHandler& getInstance();
    explicit KeyHandler(QObject *parent = nullptr);
    // void start_process(QString window_title);
    // void process_key();
    // void close_process(QString window_title);
    // void detect_mouse_status();
    void show_hide_all();
    void show_tool_bar();
private:
    static KeyHandler instance;
    // bool m_dragging = false;             // 标记是否正在拖动
    bool is_show = true;
    bool show_toolbar = false;
    // QProcess* process_mouse;
    // QStringList live_pin;
    // QSettings* myset;
signals:
    void show_all();
    void hide_all();
};

#endif // KEYHANDLER_H
