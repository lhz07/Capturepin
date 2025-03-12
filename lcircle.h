#ifndef LCIRCLE_H
#define LCIRCLE_H
#include <QPointF>

class LCircle
{
public:
    LCircle(QPointF center, double radius);
    void setRadius(double radius);
    void setCenter(QPointF center);
    QPointF center() const;
    double radius() const;
    bool contains(QPointF point);
private:
    QPointF center_point;
    double r;
};

#endif // LCIRCLE_H
