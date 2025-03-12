#ifndef PENSIZEBUTTON_H
#define PENSIZEBUTTON_H

#include <QObject>
#include <QPushButton>

class PenSizeButton : public QPushButton
{
    Q_OBJECT
public:
    explicit PenSizeButton(QWidget* parent = nullptr);
    void setPenSize(int size);
    int penSize() const;

protected:
    void paintEvent(QPaintEvent* event) override;
    void wheelEvent(QWheelEvent *event) override;
private:
    int pen_size = 10;
    int up = 0;
    int down = 0;
signals:
    void penSizeChanged();
};

#endif // PENSIZEBUTTON_H
