#include "mainwindow.h"

#include <QApplication>
#include <QtCore/QtPlugin>

int main(int argc, char *argv[])
{
    QLoggingCategory::setFilterRules(QStringLiteral("qt.qpa.wayland.textinput=false"));
    // QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QString cmd;
    if (argc >= 2)
    {
        // qDebug() << argv[1];
        cmd = argv[1];
    }
#ifdef QT_DEBUG
    QString mem_name = "CapturePin_test";
#else
    QString mem_name = "CapturePin";
#endif
    QSharedMemory* sharedMemory = new QSharedMemory(mem_name);
    QSystemSemaphore sema("QSharedMemory", 0, QSystemSemaphore::Create);
    sharedMemory->attach();
    delete sharedMemory;
    sharedMemory = new QSharedMemory(mem_name);
    if (!sharedMemory->create(sizeof(int))) {
        if (sharedMemory->error() == QSharedMemory::AlreadyExists) {
            sharedMemory->attach();
            if (sharedMemory->data() == nullptr){
                qDebug() << "It seems like you have run capturepin in a new minimal isolated environment "
                            "that doesn't carry on some \"excessive\" variables, such as \"sudo su\" \"sudo -i\", "
                            "which led to a crash of capturepin, "
                            "please reboot your computer now.";
                return 1;
            }
            sharedMemory->lock();
            int *from = (int*)sharedMemory->data();
            int *to = new int(0);
            memcpy(to, from, sizeof(int));
            qDebug() << "load sharedMemory: " << *to;
            if (*to == 1){
                if (cmd == "newshot"){
                    *to = 2;
                }else if (cmd == "toggle_show"){
                    *to = 3;
                }
                memcpy(from, to, sizeof(int));
                sema.release(1);
            }
            sharedMemory->unlock();
            sharedMemory->detach();
            return 0;
        } else {
            return 0;
        }
    }else{
        sharedMemory->lock();
        int *to = (int*)sharedMemory->data();
        const int *from = new int(1);
        memcpy(to, from, sizeof(int));
        sharedMemory->unlock();
        qDebug() << "create sharedMemory";
    }
    QApplication a(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);
    MainWindow w(sharedMemory);
    // w.show();
    //hah
    return a.exec();
}
