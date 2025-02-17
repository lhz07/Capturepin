#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <QObject>

class Screenshot : public QObject
{
    Q_OBJECT
public:
    explicit Screenshot(QObject *parent = nullptr);
    void newShot(QPixmap *&res);

};

#endif // SCREENSHOT_H
