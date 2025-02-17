#ifndef HYPRSOCKET_H
#define HYPRSOCKET_H

#include <QObject>
#include <QLocalSocket>
#include <qeventloop.h>

class HyprSocket : public QObject
{
    Q_OBJECT
public:
    explicit HyprSocket(QObject *parent = nullptr);
    ~HyprSocket();
    static HyprSocket* getInstance();
    void sendCommand(const QString &command);
    static HyprSocket* instance;

private:
    void readResponse();
    void disconnected();
    QLocalSocket *socket;
    QEventLoop loop;
    QString socketPath;
    bool can_send = true;

signals:
    void hypr_response(QString response);
};

#endif // HYPRSOCKET_H
