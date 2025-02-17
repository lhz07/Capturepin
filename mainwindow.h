#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include "start_shot.h"
#include "reactcmd.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QSharedMemory* sharedMemory, QWidget *parent = nullptr);
    ~MainWindow();
    // void setVisible(bool visible) override;

protected:
    // void closeEvent(QCloseEvent *event) override;
    // bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void pushButton_shot_clicked();
    void shot();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void checkBox_enableShortcut_checkStateChanged(int state);
    // void showMessage();
    void pushButton_browse_clicked();
    void checkBox_autoSave_checkStateChanged(int state);
private:
    Ui::MainWindow *ui;
    Start_shot* new_shot_1;
    void createActions();
    void createTrayIcon();
    void receive_shot_signal();
    QSharedMemory* sharedMemory;
    int *to;
    // QTimer* timer;
    QAction *showWindowAction;
    QAction *quitAction;
    QAction *show_hide_all;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QShortcut* shotcut;
    ReactCmd* rc;
    QThread* handle_thread;
    QSettings myset = QSettings("CapturePin", "Config");
// signals:
//     void start_process(QString window_title);
};
#endif // MAINWINDOW_H
