#include "start_shot.h"
// #include "ui_start_shot.h"
// #include "screenshot.h"
#include "pin.h"
#include "keyhandler.h"
// #include "hyprsocket.h"
// #include <QtWidgets>
#include <QWidget>
#include <QDateTime>
#include <QJsonDocument>
#include <QFile>
#include <QGuiApplication>
#include <QScreen>
#include <QMouseEvent>
#include <QClipboard>

int Start_shot::pin_count = 0;

Start_shot::Start_shot(QWidget *parent)
    : QOpenGLWidget(parent)
// , ui(new Ui::Start_shot)
{
    // ui->setupUi(this);
    // setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    // this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
    // setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowTitle("Start_shot");
    shot_socket = new HyprSocket(this);
    socket = new QLocalSocket(this);
    QAbstractSocket::connect(socket, &QLocalSocket::readyRead, this, &Start_shot::receive_socket);
    pin_pic = new QShortcut(QKeySequence("F3"), this);
    up = new QShortcut(QKeySequence("W"), this);
    copy_pic = new QShortcut(QKeySequence("Ctrl+C"), this);
    cancel = new QShortcut(QKeySequence("ESC"), this);
    connect(up, &QShortcut::activated, this, &Start_shot::move_up);
    connect(copy_pic, &QShortcut::activated, this, &Start_shot::copy);
    connect(pin_pic, &QShortcut::activated, this, &Start_shot::pin_picture);
    connect(cancel, &QShortcut::activated, this, &QWidget::close);
    // use grim
    QString run_dir = QProcessEnvironment::systemEnvironment().value("XDG_RUNTIME_DIR");
    QString img_path = run_dir + "/capturepin.ppm";
    qDebug() << "get current monitor" << QDateTime::currentDateTime().toString("hh:mm:ss:zzz");
    QString output;
    const auto gotSignal = [&output](QString response) {
        output = response;
    };
    QMetaObject::Connection conn = QObject::connect(shot_socket, &HyprSocket::hypr_response, gotSignal);
    shot_socket->sendCommand("j/activeworkspace");
    QObject::disconnect(conn);
    QJsonDocument json = QJsonDocument::fromJson(output.toUtf8());
    current_monitor = json["monitor"].toString();
    current_workspace_id = QString::number(json["id"].toDouble());
    qDebug() << current_monitor << current_workspace_id;
    qDebug() << "grim shot start" << QDateTime::currentDateTime().toString("hh:mm:ss:zzz");
    QProcess process;
    process.start("grim", QStringList() << "-o" << current_monitor << "-t" << "ppm" << img_path);
    process.waitForFinished();
    qDebug() << "grim shot finished" << QDateTime::currentDateTime().toString("hh:mm:ss:zzz");
    res = new QPixmap(img_path);
    QFile imgFile(img_path);
    imgFile.remove();

    // org.freedesktop.portal.Screenshot
    // Screenshot sc1;
    // sc1.newShot(res);


    int width = res->width();
    const auto screens = QGuiApplication::screens();
    for (const auto &s : screens){
        if (s->name() == current_monitor){
            current_screen = s;
            break;
        }
    }
    int screen_width = current_screen->size().width();
    int screen_height = current_screen->size().height();
    rd = QPoint(screen_width, screen_height);
    pixel_ratio = (double)width / screen_width;
    res->setDevicePixelRatio(pixel_ratio);
    // qDebug() << res->width();
    // qDebug() << screen->size().width();
    // qDebug() << pixel_ratio;
    // auto *quitBtn = new QPushButton("Quit", this);
    // quitBtn->setGeometry(50, 25, 100, 50);
    // this->setCentralWidget(label_pic);
    // label_pic->setScaledContents(true);
    // label_pic->setPixmap(*res);
    this->setCursor(Qt::CrossCursor);
    this->setMouseTracking(true);
    // qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss:zzz");
    this->resize(screen_width, screen_height);
    this->show();
}

Start_shot::~Start_shot()
{
    // delete ui;
    // delete label_pic;
    delete pin_pic;
    delete res;
    delete copy_pic;
    delete cancel;
}

void Start_shot::window_shown()
{
    this->setScreen(current_screen);
    // qDebug() << this->screen();
    QSettings myset;
    if (myset.value("fullscreen").toBool()){
        shot_socket->sendCommand("dispatch fullscreen 0");
    }
}

void Start_shot::receive_socket()
{
    QString event = socket->readAll();
    const QStringList events = event.split('\n');
    for (const QString &i : events) {
        if (i.contains("openwindow") && i.endsWith(this->windowTitle())){
            socket->disconnectFromServer();
            qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << this->windowTitle() << "get show window!";
            window_shown();
        }
    }
}

void Start_shot::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    //用保存的全屏对象实例化背景
    painter.setBackground(QBrush(*res));
    painter.setBackgroundMode(Qt::OpaqueMode);
    QRect rect(QPoint(0, 0), current_screen->size());
    painter.eraseRect(rect);
    QPixmap pix(current_screen->size());
    //用灰色填充pix
    pix.fill((QColor(10, 10, 10, 140)));
    painter.drawPixmap(0,0,pix);//把灰色模糊绘制到背景图片上
    QPen pen;
    pen.setColor(QColor(44, 172, 224));
    pen.setWidth(2);
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
    QRect rect_draw(p_start, p_end);
    painter.eraseRect(rect_draw);
    painter.drawRect(rect_draw);
    if (!draw_completed){
        this->setCursor(Qt::ArrowCursor);
        this->setCursor(Qt::CrossCursor);
    }
}

void Start_shot::pin_picture()
{
    // qDebug() << "pressed!";
    this->setCursor(Qt::ArrowCursor);
    close();
    QRect cropRect(p_start*pixel_ratio, p_end*pixel_ratio);
    QPixmap croppedPix = res->copy(cropRect);
    auto_save(croppedPix);
    pin1 = new Pin(croppedPix, pin_pic_pos);
    pin1->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint |Qt::Tool);
    pin1->setAttribute(Qt::WA_TranslucentBackground);
    pin1->setWindowTitle("PinnedScreenshot." + QString::number(++pin_count));
    QSettings myset = QSettings("CapturePin", "Config");
    if (myset.value("DE") == "Hyprland")
    {
        emit start_process(pin1->windowTitle());
    }
    connect(pin1, &Pin::show_tool_bar, &KeyHandler::getInstance(), &KeyHandler::show_tool_bar);
    connect(&KeyHandler::getInstance(), &KeyHandler::show_all, pin1, &Pin::show_all);
    connect(&KeyHandler::getInstance(), &KeyHandler::hide_all, pin1, &Pin::hide_all);
    pin1->setAttribute(Qt::WA_DeleteOnClose);
    pin1->show();
}

void Start_shot::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        // qDebug() << "press";
        // qDebug() << event->globalPosition().toPoint();
        QPoint temp = event->pos();
        if (!draw_completed){
            p_start = temp;
        }else{
            if (p_start.x() < temp.x() && p_start.y() < temp.y() && p_end.x() > temp.x() && p_end.y() > temp.y()){
                //图片内部
                cursor_pos = temp;
            }else if (temp.x() >= p_end.x() && p_start.y() <= temp.y() && p_end.y() >= temp.y()){
                //图片右侧
                p_end.rx() = temp.x();
                right_move = true;
            }else if (temp.x() <= p_start.x() && p_start.y() <= temp.y() && p_end.y() >= temp.y()){
                //图片左侧
                p_start.rx() = temp.x();
                left_move = true;
            }else if (temp.x() >= p_start.x() && temp.x() <= p_end.x() && p_start.y() >= temp.y()){
                //图片上侧
                p_start.ry() = temp.y();
                up_move = true;
            }else if (temp.x() >= p_start.x() && temp.x() <= p_end.x() && p_end.y() <= temp.y()){
                //图片下侧
                p_end.ry() = temp.y();
                down_move = true;
            }else if (temp.x() >= p_end.x() && p_start.y() >= temp.y()){
                //图片右上
                p_end.rx() = temp.x();
                p_start.ry() = temp.y();
                right_move = true;
                up_move = true;
            }else if (temp.x() >= p_end.x() && p_end.y() <= temp.y()){
                //图片右下
                p_end.rx() = temp.x();
                p_end.ry() = temp.y();
                right_move = true;
                down_move = true;
            }else if (temp.x() <= p_start.x() && temp.y() <= p_start.y()){
                //图片左上
                p_start.rx() = temp.x();
                p_start.ry() = temp.y();
                left_move = true;
                up_move = true;
            }else if (temp.x() <= p_start.x() && temp.y() >= p_end.y()){
                //图片左下
                p_start.rx() = temp.x();
                p_end.ry() = temp.y();
                left_move = true;
                down_move = true;
            }
            this->update();
        }
    }
}

void Start_shot::mouseMoveEvent(QMouseEvent *event)
{
    QPoint temp = event->pos();
    if (event->buttons() & Qt::LeftButton)
    {
        if (!draw_completed){
            p_end = temp;
        }else if (right_move || left_move || up_move || down_move){
            if (right_move){
                p_end.rx() = temp.x();
            }else if (left_move){
                p_start.rx() = temp.x();
            }
            if (up_move){
                p_start.ry() = temp.y();
            }else if (down_move){
                p_end.ry() = temp.y();
            }
        }else{
            QPoint cmp = temp - cursor_pos;
            cursor_pos = temp;
            p_start += cmp;
            p_end += cmp;
            if (p_start.x() < 0){
                p_start.rx() = 0;
                p_end.rx() -= cmp.x();
            }else if (p_end.x() > rd.x()){
                p_end.rx() = rd.x();
                p_start.rx() -= cmp.x();
            }
            if (p_start.y() < 0){
                p_start.ry() = 0;
                p_end.ry() -= cmp.y();
            }else if (p_end.y() > rd.y()){
                p_end.ry() = rd.y();
                p_start.ry() -= cmp.ry();
            }
        }
        this->update();
    }else if(draw_completed){
        if (p_start.x() < temp.x() && p_start.y() < temp.y() && p_end.x() > temp.x() && p_end.y() > temp.y()){
            //图片内部
            this->setCursor(Qt::SizeAllCursor);
        }else if (temp.x() >= p_end.x() && p_start.y() <= temp.y() && p_end.y() >= temp.y()){
            this->setCursor(Qt::SizeHorCursor);
        }else if (temp.x() <= p_start.x() && p_start.y() <= temp.y() && p_end.y() >= temp.y()){
            this->setCursor(Qt::SizeHorCursor);
        }else if (temp.x() >= p_start.x() && temp.x() <= p_end.x() && p_start.y() >= temp.y()){
            this->setCursor(Qt::SizeVerCursor);
        }else if (temp.x() >= p_start.x() && temp.x() <= p_end.x() && p_end.y() <= temp.y()){
            this->setCursor(Qt::SizeVerCursor);
        }else if (temp.x() >= p_end.x() && p_start.y() >= temp.y()){
            //图片右上
            this->setCursor(Qt::SizeBDiagCursor);
        }else if (temp.x() >= p_end.x() && p_end.y() <= temp.y()){
            //图片右下
            this->setCursor(Qt::SizeFDiagCursor);
        }else if (temp.x() <= p_start.x() && temp.y() <= p_start.y()){
            //图片左上
            this->setCursor(Qt::SizeFDiagCursor);
        }else if (temp.x() <= p_start.x() && temp.y() >= p_end.y()){
            //图片左下
            this->setCursor(Qt::SizeBDiagCursor);
        }else{
            this->setCursor(Qt::ArrowCursor);
        }
    }
}

void Start_shot::showEvent(QShowEvent *event)
{
    QString runtimeDir = qEnvironmentVariable("XDG_RUNTIME_DIR");
    QString socketPath = QString("%1/hypr/%2/.socket2.sock")
                             .arg(runtimeDir, qEnvironmentVariable("HYPRLAND_INSTANCE_SIGNATURE"));
    socket->connectToServer(socketPath);
}

void Start_shot::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !draw_completed)
    {
        // qDebug() << "release";
        // qDebug() << event->globalPosition().toPoint();
        correct_where_to_start(p_start, p_end);
        pin_pic_pos = this->mapToGlobal(p_start);
        // qDebug() << p_start << pin_pic_pos;
        draw_completed = true;
        this->setCursor(Qt::ArrowCursor);
    }else if (event->button() == Qt::LeftButton && (right_move || left_move || up_move || down_move)){
        right_move = false;
        left_move = false;
        down_move = false;
        up_move = false;
        correct_where_to_start(p_start, p_end);
        pin_pic_pos = this->mapToGlobal(p_start);
        // qDebug() << p_start << pin_pic_pos;
    }else{
        pin_pic_pos = this->mapToGlobal(p_start);
        // qDebug() << p_start << pin_pic_pos;
    }
}

void Start_shot::correct_where_to_start(QPoint &p_start, QPoint &p_end)
{
    QPoint temp(0, 0);
    if (p_start.x() < p_end.x() && p_start.y() < p_end.y())
    {}
    else if (p_start.x() > p_end.x()){
        if (p_start.y() > p_end.y()){
            temp = p_start;
            p_start = p_end;
            p_end = temp;
        }else{
            int width = p_start.x() - p_end.x();
            p_start.rx() = p_start.x() - width;
            p_end.rx() = p_end.x() + width;
        }
    }else{
        int height = p_start.y() - p_end.y();
        p_start.ry() = p_start.y() - height;
        p_end.ry() = p_end.y() + height;
    }
}

void Start_shot::copy()
{
    if (draw_completed){
        QClipboard *clipboard = QGuiApplication::clipboard();
        this->setCursor(Qt::ArrowCursor);
        QRect cropRect(p_start*pixel_ratio, p_end*pixel_ratio);
        QPixmap croppedPix = res->copy(cropRect);
        auto_save(croppedPix);
        clipboard->setPixmap(croppedPix);
        close();
    }
}

void Start_shot::auto_save(QPixmap pic)
{
    QSettings myset;
    if (myset.value("enableAutoSave").toBool()){
        QString name = QString("Capturepin_%1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss"));
        pic.save(myset.value("default_path").toString()+'/'+name+".png", "png");
    }
}

void Start_shot::move_up()
{
    if (draw_completed && p_start.y() - 1 >= 0){
        p_start.ry() -= 1;
        p_end.ry() -= 1;
        this->update();
    }
}

void Start_shot::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Up){
        start_move = true;
        if (draw_completed && p_start.y() - 1 >= 0){
            p_start.ry() -= 1;
            p_end.ry() -= 1;
            this->update();
        }
    }
}

void Start_shot::keyReleaseEvent(QKeyEvent *event)
{
    start_move = false;
}






