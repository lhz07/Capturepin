#include "reactcmd.h"
#include <QSystemSemaphore>
#include <QDateTime>

ReactCmd::ReactCmd() {}

void ReactCmd::run()
{
    QSystemSemaphore* sema = new QSystemSemaphore("QSharedMemory", 0);
    while (1){
        sema->acquire();
        qDebug() << "got signal" << QDateTime::currentDateTime().toString("hh:mm:ss:zzz");
        emit memoryChanged();
    }
}
