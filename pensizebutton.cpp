#include "pensizebutton.h"
#include <QPainter>
#include <QWheelEvent>

PenSizeButton::PenSizeButton(QWidget* parent) : QPushButton(parent)
{
    this->setFixedSize(30, 30);
}

void PenSizeButton::setPenSize(int size)
{
    this->pen_size = size;
}

int PenSizeButton::penSize() const
{
    return this->pen_size;
}

void PenSizeButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawEllipse(this->rect().center().toPointF(), pen_size / 2.0, pen_size / 2.0);
    // painter.setPen(Qt::black);
    // painter.setBrush(Qt::NoBrush);
    // painter.drawRect(0, 0, 30, 30);
    // QPushButton::paintEvent(event);
}

void PenSizeButton::wheelEvent(QWheelEvent *event)
{
    QPushButton::wheelEvent(event);
    int temp = event->angleDelta().y();
    if (temp > 0) {
        if (temp >= 110){
            if (pen_size + 1 <= 100){
                pen_size++;
                emit penSizeChanged();
                this->update();
            }
        }else if (up + temp >= 110){
            if (pen_size + 1 <= 100){
                pen_size++;
                emit penSizeChanged();
                this->update();
            }
            up = 0;
        }else{
            up += temp;
        }
    } else {
        if (temp <= -110){
            if (pen_size - 1 >= 1){
                pen_size--;
                emit penSizeChanged();
                this->update();
            }
        }else if (down + temp <= -110){
            if (pen_size - 1 >= 1){
                pen_size--;
                emit penSizeChanged();
                this->update();
            }
            down = 0;
        }else{
            down += temp;
        }
    }
}
