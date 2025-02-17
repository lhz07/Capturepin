#include "hyprsocket.h"

HyprSocket* HyprSocket::instance;

HyprSocket::HyprSocket(QObject *parent)
    : QObject{parent}, socket(new QLocalSocket(this))
{
    QString runtimeDir = qEnvironmentVariable("XDG_RUNTIME_DIR");
    socketPath = QString("%1/hypr/%2/.socket.sock")
                     .arg(runtimeDir, qEnvironmentVariable("HYPRLAND_INSTANCE_SIGNATURE"));

    // 连接到 socket
    connect(socket, &QLocalSocket::disconnected, this, &HyprSocket::disconnected);
    connect(socket, &QLocalSocket::readyRead, this, &HyprSocket::readResponse);
}

HyprSocket::~HyprSocket()
{
    socket->deleteLater();
}

HyprSocket* HyprSocket::getInstance()
{
    return instance;
}

void HyprSocket::sendCommand(const QString &command) {
    // qDebug() << command;
    if (can_send){
        socket->connectToServer(socketPath);
        can_send = false;
        if (socket->state() == QLocalSocket::ConnectedState) {
            socket->write(command.toUtf8());
            // qDebug() << command;
            socket->flush();
            loop.exec();

            // if (!socket->waitForBytesWritten(3000)) {
            //     qDebug() << "发送命令失败：" << socket->errorString();
            // } else {
            //     qDebug() << "命令发送成功：" << command;
            // }
        } else {
            qDebug() << "Socket 未连接，无法发送命令。";
        }
    }else{
        // qDebug() << "cannot send now!";
    }
}

void HyprSocket::readResponse() {
    QByteArray response = socket->readAll();
    // qDebug() << "收到返回值：" << QString::fromUtf8(response);
    emit hypr_response(QString::fromUtf8(response));
}

void HyprSocket::disconnected()
{
    can_send = true;
    loop.quit();
}
