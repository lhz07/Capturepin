#ifndef SHAREVAR_H
#define SHAREVAR_H

#include <QObject>
enum class Tools{NONE, TEXT, RECTANGLE, LINE, ERASER};

class ShareVar : public QObject
{
    Q_OBJECT
public:
    explicit ShareVar(QObject *parent = nullptr);
    static bool is_selected;
    static const int default_font_size = 24;
    static double device_pixel_ratio;

signals:
};

#endif // SHAREVAR_H
