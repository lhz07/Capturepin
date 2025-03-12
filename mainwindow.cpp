#include "mainwindow.h"
#include "./ui_mainwindow.h"
// #include "screenshot.h"
#include "start_shot.h"
#include "keyhandler.h"
// #include <QtWidgets>
#include <QDir>
#include <QMenu>
#include <QFileDialog>
#include <unistd.h>
#include <QWidget>
#include <QMessageBox>
#include "hyprsocket.h"

MainWindow::MainWindow(QSharedMemory* sharedMemory, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // QSurfaceFormat format;
    // format.setRenderableType(QSurfaceFormat::OpenGL);
    // QSurfaceFormat::setDefaultFormat(format);
    HyprSocket::instance = new HyprSocket();
    // HyprSocket::getInstance().sendCommand("clients");
    QString desktopEnv = QProcessEnvironment::systemEnvironment().value("XDG_SESSION_DESKTOP");
    qDebug() << desktopEnv;
    // ShareVar::hypr_socket->sendCommand("clients");
    // ShareVar::hypr_socket->sendCommand("clients");
    // handle_thread = new QThread();
    // key_hdl->moveToThread(handle_thread);
    // handle_thread->start();
    myset.setValue("DE", desktopEnv);
    // myset.setValue("enableAutoSave", 0);
    // if (!myset.contains("enableShortcut")){
    //     myset.setValue("enableShortcut", 1);
    // }
    myset.setValue("enableShortcut", myset.value("enableShortcut", true).toBool());
    myset.setValue("enableAutoSave", myset.value("enableAutoSave", false).toBool());
    myset.setValue("default_path", myset.value("default_path", QDir::homePath() + "/Pictures").toString());
    myset.setValue("fullscreen", myset.value("fullscreen", true).toBool());
    ui->checkBox_enableShortcut->setChecked(myset.value("enableShortcut").toBool());
    ui->checkBox_autoSave->setChecked(myset.value("enableAutoSave").toBool());
    ui->lineEdit_default_path->setText(myset.value("default_path").toString());
    ui->checkBox_fullscreen->setChecked(myset.value("fullscreen").toBool());
    connect(ui->pushButton_shot, &QPushButton::clicked, this, &MainWindow::pushButton_shot_clicked);
    connect(ui->checkBox_enableShortcut, &QCheckBox::toggled, this, &MainWindow::checkBox_enableShortcut_toggled);
    connect(ui->checkBox_autoSave, &QCheckBox::toggled, this, &MainWindow::checkBox_autoSave_toggled);
    connect(ui->checkBox_fullscreen, &QCheckBox::toggled, this, &MainWindow::checkBox_fullscreen_toggled);
    connect(ui->pushButton_browse, &QPushButton::clicked, this, &MainWindow::pushButton_browse_clicked);
    createActions();
    createTrayIcon();
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
#ifdef QT_DEBUG
    trayIcon->setIcon(QIcon(":/pic/resource/pic/test.svg"));
#else
    trayIcon->setIcon(QIcon(":/pic/resource/pic/screenie.svg"));
#endif
    trayIcon->show();
    // timer = new QTimer();
    to = new int(0);
    this->sharedMemory = sharedMemory;
    rc = new ReactCmd();
    rc->start();
    connect(rc, &ReactCmd::memoryChanged, this, &MainWindow::receive_shot_signal);
    sharedMemory->attach();
    // qApp->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    rc->deleteLater();
    // handle_thread->deleteLater();
    delete to;
}

// bool MainWindow::eventFilter(QObject *watched, QEvent *event)
// {
//     QList<enum QEvent::Type> types({QEvent::MouseMove, QEvent::MetaCall, QEvent::ChildAdded, QEvent::ChildRemoved, QEvent::SockAct});
//     if (event->type() == QEvent::MouseButtonPress){
//         QMouseEvent* temp = static_cast<QMouseEvent*>(event);
//         if (temp->button() == Qt::LeftButton){
//             qDebug() << event->type() << watched->objectName() << watched->metaObject()->className();
//         }
//     }
//     return QObject::eventFilter(watched, event);
// }

void MainWindow::receive_shot_signal()
{
    int *from = (int*)sharedMemory->data();
    *to = 0;
    memcpy(to, from, sizeof(int));
    if (*to != 1){
        if (*to == 2){
            MainWindow::shot();
        }else if (*to == 3){
            KeyHandler::getInstance().show_hide_all();
        }
        *to = 1;
        sharedMemory->lock();
        memcpy(from, to, sizeof(int));
        sharedMemory->unlock();
    }
}

void MainWindow::pushButton_shot_clicked()
{
    hide();
    usleep(2000000);
    // new_shot_1 = new Start_shot();
    // connect(new_shot_1, &Start_shot::start_process, key_hdl, &KeyHandler::start_process);
    // new_shot_1->setAttribute(Qt::WA_DeleteOnClose);
    // new_shot_1->show();
    MainWindow::shot();
}

void MainWindow::shot()
{
    new_shot_1 = new Start_shot();
    // new_shot_1->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint |Qt::Tool);
    // new_shot_1->setWindowTitle("PinnedScreenshot");
    // QScreen *screen = QGuiApplication::primaryScreen();
    // int width = screen->size().width();
    // int height = screen->size().height();
    // new_shot_1->resize(width, height);
    // connect(new_shot_1, &Start_shot::start_process, key_hdl, &KeyHandler::start_process);
    new_shot_1->setAttribute(Qt::WA_DeleteOnClose);
    new_shot_1->show();
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason){
    case QSystemTrayIcon::Trigger:
        MainWindow::shot();
        break;
    case QSystemTrayIcon::DoubleClick:
        break;
    case QSystemTrayIcon::MiddleClick:
        break;
    default:
        break;
    }
}

void MainWindow::createActions()
{
    showWindowAction = new QAction("显示窗口", this);
    connect(showWindowAction, &QAction::triggered, this, &QWidget::showNormal);
    quitAction = new QAction("退出程序", this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    show_hide_all = new QAction("显示/隐藏所有贴图");
    connect(show_hide_all, &QAction::triggered, &KeyHandler::getInstance(), &KeyHandler::show_hide_all);
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(showWindowAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(show_hide_all);
    trayIconMenu->addAction(quitAction);
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void MainWindow::pushButton_browse_clicked()
{
    QFileDialog::Options options;
    options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
    QString directory = QFileDialog::getExistingDirectory(this,
                                                          tr("QFileDialog::getExistingDirectory()"),
                                                          ui->lineEdit_default_path->text(),
                                                          options);
    if (!directory.isEmpty()){
        ui->lineEdit_default_path->setText(directory);
        myset.setValue("default_path", directory);
        myset.sync();
    }
}

void MainWindow::checkBox_fullscreen_toggled(bool checked)
{
    qDebug() << checked;
    myset.setValue("fullscreen", checked);
    myset.sync();
}

void MainWindow::checkBox_enableShortcut_toggled(bool checked)
{
    myset.setValue("enableShortcut", checked);
    myset.sync();
}

void MainWindow::checkBox_autoSave_toggled(bool checked)
{
    myset.setValue("enableAutoSave", checked);
    myset.sync();
}


void MainWindow::on_pushButton_clicked()
{
    QMessageBox::aboutQt(this, "about");
}

