#ifndef SHAREVAR_H
#define SHAREVAR_H

#include <QObject>

class ShareVar : public QObject
{
    Q_OBJECT
public:
    explicit ShareVar(QObject *parent = nullptr);
    static bool is_selected;
    static const int default_font_size = 24;

signals:
};

#endif // SHAREVAR_H
