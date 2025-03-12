#include "lcircle.h"
#include <QLineF>

LCircle::LCircle(QPointF center, double radius) : center_point(center), r(radius)
{}

void LCircle::setRadius(double radius)
{
    this->r = radius;
}

void LCircle::setCenter(QPointF center)
{
    this->center_point = center;
}

QPointF LCircle::center() const
{
    return this->center_point;
}

double LCircle::radius() const
{
    return this->r;
}

bool LCircle::contains(QPointF point)
{
    if (QLineF(this->center_point, point).length() <= this->r){
        return true;
    }else {
        return false;
    }
}
