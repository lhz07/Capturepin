#include "sharevar.h"

ShareVar::ShareVar(QObject *parent)
    : QObject{parent}
{}

 bool ShareVar::is_selected = false;
