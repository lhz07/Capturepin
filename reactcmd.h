#ifndef REACTCMD_H
#define REACTCMD_H
// #include <QtWidgets>
#include <QThread>
class ReactCmd : public QThread
{
    Q_OBJECT
public:
    ReactCmd();
protected:
    void run() override;
signals:
    void memoryChanged();
};

#endif // REACTCMD_H
