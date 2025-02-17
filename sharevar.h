#ifndef SHAREVAR_H
#define SHAREVAR_H

#include <QObject>

class ShareVar : public QObject
{
    Q_OBJECT
public:
    explicit ShareVar(QObject *parent = nullptr);
    static bool is_selected;

signals:
};

#endif // SHAREVAR_H
