#include "reactcmd.h"

ReactCmd::ReactCmd() {}

void ReactCmd::run()
{
    QSystemSemaphore* sema = new QSystemSemaphore("QSharedMemory", 0);
    while (1){
        sema->acquire();
        emit memoryChanged();
    }
}
