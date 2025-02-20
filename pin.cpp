#include "pin.h"
#include "start_shot.h"
#include <QtWidgets>
#include <unistd.h>
#include "hyprsocket.h"

// #include <QDBusInterface>
// #include <QDBusReply>
// #include "ui_pin.h"

Pin::Pin(QPixmap pic, QPoint p_start, QWidget *parent)
    : QWidget(parent)
// , ui(new Ui::Pin)
{
    // ui->setupUi(this);
    socket = new QLocalSocket(this);
    pin_socket = new HyprSocket(this);
    QAbstractSocket::connect(socket, &QLocalSocket::readyRead, this, &Pin::receive_socket);
    scene = new QGraphicsScene(this);
    view = new GraphView(scene, this);
    toolbar = new ToolBar(scene, "PinnedScreenshot." + QString::number(Start_shot::pin_count + 1));
    toolbar->setWindowTitle("capturepinTool." + QString::number(Start_shot::pin_count + 1));
    toolbar->resize(500, 65); //used 52
    connect(this, &Pin::show_tool_bar, toolbar, &ToolBar::show_toolbar);
    connect(this, &Pin::restore_toolbar, toolbar, &ToolBar::restore_toolbar);
    connect(toolbar, &ToolBar::toggle_toolbar_visible, this, &Pin::toggle_toolbar_visible);
    this->pic = pic;
    this->p_start = p_start;
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    label_pic = new QLabel(this);
    label_pic->setPixmap(pic);
    // bar = new QToolBar("capturepinTool.1");
    mainLayout->addWidget(label_pic, 0);
    label_pic->lower();
    paint_count = new int(0);
    copy_pic = new QShortcut(QKeySequence("Ctrl+C"), this);
    // add_text = new QShortcut(QKeySequence("Shift+P"), this);
    key_show_bar = new QShortcut(QKeySequence("Space"), this);
    myset.setValue("toggle_toolbar_visible", "Space");
    // connect(add_text, &QShortcut::activated, this, &Pin::addText);
    connect(copy_pic, &QShortcut::activated, this, &Pin::copy);
    connect(key_show_bar, &QShortcut::activated, this, &Pin::toggle_toolbar_visible);
    menu = new QMenu(this);
    quickSaveAction = new QAction("Quick save", this);
    saveAction = new QAction("Save picture", this);
    copyAction = new QAction("Copy picture", this);
    closeAction = new QAction("Close", this);
    toolbarAction = new QAction("Show/Hide toolbar", this);
    // text_color = new QAction("Color");
    // addTextAction = new QAction("T", this);
    // addTextAction->setCheckable(true);
    connect(quickSaveAction, &QAction::triggered, this, &Pin::quick_save);
    connect(saveAction, &QAction::triggered, this, &Pin::save_pic);
    connect(copyAction, &QAction::triggered, this, &Pin::copy);
    connect(closeAction, &QAction::triggered, this, &QWidget::close);

    connect(closeAction, &QAction::triggered, toolbar, &QWidget::close);
    connect(toolbarAction, &QAction::triggered, this, &Pin::toggle_toolbar_visible);
    // connect(addTextAction, &QAction::triggered, this, &Pin::addText);
    // connect(text_color, &QAction::triggered, this, &Pin::color_pick);
    menu->addAction(copyAction);
    menu->addAction(toolbarAction);
    menu->addAction(quickSaveAction);
    menu->addAction(saveAction);
    menu->addAction(closeAction);
    QString w_h = QString::number(pic.size().width()) + "×" + QString::number(pic.size().height());
    menu->addAction(w_h);
    // bar->addAction(saveAction);
    // bar->addAction(closeAction);
    // bar->addAction(addTextAction);

    connect(view, &GraphView::mouse_clicked, toolbar, &ToolBar::screen_clicked);
    connect(toolbar, &ToolBar::view_visible, view, &GraphView::text_tool_visible);
    // connect(toolbar, &ToolBar::send_current_tool, view, &GraphView::receive_current_tool);
    // view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setFrameShape(QFrame::NoFrame);
    view->hide();
    // view->setGeometry(0, 0, this->size().width(), this->size().height());

    view->setStyleSheet("background: transparent;");


    // process_mouse = new QProcess();
    // connect(process_mouse, &QProcess::readyReadStandardOutput, this, &Pin::detect_mouse_status);
    // process_mouse->start("bash", QStringList() << "-c" << "libinput debug-events");
    // QProcess process;
    // // process.setProgram("bash");
    // process.start("bash", QStringList() << "-c" << "hyprctl clients");
    // process.waitForStarted();
    // process.waitForFinished();
    // QString output = process.readAll();

    // // 简单解析窗口 ID
    // QStringList lines = output.split("\n");
    // QString windowId;
    // QString pos;
    // QString window_title = "PinnedScreenshot";
    // for (int i = 0; i < lines.size(); ++i) {
    //     if (lines[i].contains("title: " + window_title)) {
    //         for (int j = i-1; j >= 0; --j) {
    //             if (lines[j].contains("at: ")) {
    //                 pos = lines[j].split(": ").last().trimmed();
    //                 break;
    //             }
    //         }
    //         for (int j = i; j < lines.size(); ++j) {
    //             if (lines[j].contains("pid: ")) {
    //                 windowId = lines[j].split(": ").last().trimmed();
    //                 break;
    //             }
    //         }
    //         break;
    //     }
    // }
    // qDebug() << windowId;
    // qDebug() << pos;
    // this->setMouseTracking(true);
    // // 设置窗口为矩形
    // QBitmap mask(size());
    // mask.fill(Qt::color1);
    // QPainter painter(&mask);
    // painter.setBrush(Qt::color0);
    // painter.drawRoundedRect(rect(), 0, 0); // 圆角半径为 0
    // painter.end();
    // setMask(mask);
    // label_pic->setScaledContents(true);
    // this->showFullScreen();
}

Pin::~Pin()
{
    // delete ui;
    delete mainLayout;
    delete label_pic;
    delete copy_pic;
    delete menu;
    delete saveAction;
    delete copyAction;
    delete closeAction;
    toolbar->deleteLater();
    if (m_dragging){
        QProcess process;
        QStringList list1;
        list1 << "-c" << "ydotool key 125:0 56:0";
        process.startDetached("bash", list1);
    }
    if (myset.value("DE") == "Hyprland"){
        emit close_process(this->window_title);
    }
}

void Pin::copy()
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setPixmap(pic);
    // qDebug() << "copied";
}

void Pin::contextMenuEvent(QContextMenuEvent *)
{
    menu->exec(cursor().pos());
}

void printObjectChild(const QObject *obj, int spaceCount)
{
    qDebug() << QString("%1%2 : %3")
    .arg("", spaceCount)
        .arg(obj->metaObject()->className(), obj->objectName());

    QObjectList childs = obj->children();
    foreach (QObject *child, childs) {
        printObjectChild(child, spaceCount + 2);
    }
}

void Pin::quick_save()
{
    QString name = QString("Capturepin_%1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss"));
    pic.save(myset.value("default_path").toString()+'/'+name+".png", "png");
}

void Pin::receive_socket()
{
    QString event = socket->readAll();
    QStringList events = event.split('\n');
    foreach (QString i, events) {
        if (i.contains("openwindow") && i.endsWith(window_title)){
            socket->disconnectFromServer();
            qDebug() << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << this->windowTitle() << "get show window!";
            window_shown();
        }
    }
}

void Pin::window_shown()
{
    if (get_id && !is_hide)
    {
        (*paint_count)++;
        if (*paint_count == 1){
            // 窗口显示后执行操作
            view->setFixedSize(this->size());
            scene->setSceneRect(0, 0, view->width(), view->height());
            // QProcess process;
            QString left_top = QString::number(p_start.x()) + " " + QString::number(p_start.y()) + "," + "title:";
            // QStringList list1;
            // list1 << "-c" << "hyprctl dispatch movewindowpixel exact " + left_top + window_title;
            // process.start("bash", list1);
            // process.waitForFinished();
            this->move(p_start);
            pin_socket->sendCommand("dispatch movewindowpixel exact " + left_top + window_title);
            pin_socket->sendCommand("dispatch setprop title:" + window_title + " norounding 1");
            pin_socket->sendCommand("dispatch setprop title:" + window_title + " noanim 1");
            // process.start("bash", QStringList() << "-c" << "hyprctl dispatch setprop title:" + window_title + " norounding 1");
            // process.waitForFinished();
            this->origin_size = this->size();
            get_id = false;
            *paint_count = 0;
        }
    }
    else if (is_hide)
    {
        QString temp_cmd = pos.replace(',', ' ') +  ",title:";
        // QProcess process;
        // QStringList list1;
        // list1 << "-c" << "hyprctl dispatch movewindowpixel exact " + temp_cmd + window_title;
        // qDebug() << list1;
        // process.start("bash", list1);
        // process.waitForFinished();
        QProcess process;
        process.startDetached("bash", QStringList() << "-c" << "hyprctl dispatch movewindowpixel exact " + temp_cmd + window_title);
        process.startDetached("bash", QStringList() << "-c" << "hyprctl dispatch setprop title:" + window_title + " norounding 1");
        process.startDetached("bash", QStringList() << "-c" << "hyprctl dispatch setprop title:" + window_title + " noanim 1");
        // pin_socket->sendCommand("dispatch movewindowpixel exact " + temp_cmd + window_title);
        // pin_socket->sendCommand("dispatch setprop title:" + window_title + " norounding 1");
        // pin_socket->sendCommand("dispatch setprop title:" + window_title + " noanim 1");
        // HyprSocket::getInstance()->sendCommand("dispatch movewindowpixel exact " + temp_cmd + window_title);
        // HyprSocket::getInstance()->sendCommand("dispatch setprop title:" + window_title + " norounding 1");
        // HyprSocket::getInstance()->sendCommand("dispatch setprop title:" + window_title + " noanim 1");
        if (to_show_bar){
            show_toolbar();
            to_show_bar = false;
        }
        // process.start("bash", QStringList() << "-c" << "hyprctl dispatch setprop title:" + window_title + " norounding 1");
        // process.waitForFinished();
        is_hide = false;
    }
}

void Pin::save_pic()
{
    QString name = QString("Capturepin_%1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss"));
    QString file_path = QFileDialog::getSaveFileName(this, "Save Picture", QDir::homePath()+'/'+name, "PNG Files (*.png)");
    if (file_path != ""){
        if (!file_path.endsWith(".png")) {
            file_path += ".png";
        }
        pic.save(file_path, "png");
    }
}


void Pin::mousePressEvent(QMouseEvent *event)
{
    // this->move(0, 0);
    // this->update();
    // ydotool method:
    if (event->button() == Qt::LeftButton) {
        if (myset.value("DE") == "Hyprland"){
            // QProcess process;
            // process.startDetached("bash", QStringList() << "-c" << "ydotool key 125:1 56:1");
            // m_dragging = true;
            QString output;
            const auto gotSignal = [&output](QString response) {
                output = response;
            };
            auto temp_socket = new HyprSocket();
            QMetaObject::Connection conn = QObject::connect(temp_socket, &HyprSocket::hypr_response, gotSignal);
            temp_socket->sendCommand("clients");
            QStringList lines = output.split("\n");
            for (int i = 0; i < lines.size(); ++i) {
                if (lines[i].endsWith("title: " + window_title)) {
                    for (int j = i-1; j >= 0; --j) {
                        if (lines[j].contains("at: ")) {
                            pos = lines[j].split(": ").last().trimmed();
                            qDebug() << pos;
                            break;
                        }
                    }
                    break;
                }
            }
            start_move = QPoint(pos.split(",")[0].toInt(), pos.split(",")[1].toInt());
            temp_socket->sendCommand("cursorpos");
            QObject::disconnect(conn);
            qDebug() << "pin press!";
            qDebug() << view->rect() << view->isVisible();
            output.remove(" ");
            auto old_cursor_pos = QPoint(output.split(",")[0].toInt(), output.split(",")[1].toInt());
            m_dragPosition = old_cursor_pos - start_move;
            qDebug() << start_move;
            qDebug() << m_dragPosition;
            temp_socket->deleteLater();
            // emit process_key();
        }
        else {
            QWindow *window = this->windowHandle();
            // m_dragPosition = event->globalPosition().toPoint() - window->position();
            if (window) {
                bool result = window->startSystemMove(); // 触发系统移动
                qDebug() << "Start system move triggered:" << result;
            }
        }
        // process_mouse->waitForStarted();
        // qDebug() << process_mouse->readAllStandardOutput();
        // delete process_mouse;
    }
    // process_mouse = new QProcess();
    // process_mouse->start("bash", QStringList() << "-c" << "libinput debug-events|grep BTN_LEFT");
    // process_mouse->waitForStarted();
    // connect(process_mouse, &QProcess::readyReadStandardOutput, this, &Pin::detect_mouse_status);
    // qDebug() << process_mouse->readAllStandardOutput();
    // start_move = event->globalPosition().toPoint();
    // if (m_dragging){
    //     process.start("bash", QStringList() << "-c" << "ydotool key 125:0 56:0");
    //     process.waitForFinished();
    // }else{
    //     m_dragging = true;
    // }
    // process.startDetached("bash", QStringList() << "-c" << "ydotool key 125:1 56:1");
    // end

    // process.startDetached("bash", QStringList() << "-c" << "ydotool click 0xc0");
    // QString left_top = QString::number(p_start.x()) + " " + QString::number(p_start.y()) + "," + "title:";
    // QString pid = "pid:" + windowId;
    // QString output = process.readAll();
    // qDebug() << list1;
    // qDebug() << output;
    // m_dragging = true;
    // m_dragPosition = event->globalPosition().toPoint();
    // start_move = QPoint(pos.split(",")[0].toInt(), pos.split(",")[1].toInt());
    // qDebug() << m_dragPosition;
    // qDebug() << start_move;
    // qDebug() << this->mapToGlobal(QPoint(0, 0));

    // QProcess process;
    // QString command = QString("hyprctl dispatch focuswindow %1 && hyprctl dispatch move %2 %3")
    //                       .arg(windowID)
    //                       .arg(x)
    //                       .arg(y);
    // process.start("bash", QStringList() << "-c" << command);
    // process.waitForFinished();
    // }
    QWidget::mousePressEvent(event);
}

void Pin::mouseMoveEvent(QMouseEvent *event)
{
    QString output;
    const auto gotSignal = [&output](QString response) {
        output = response;
    };
    QMetaObject::Connection conn = QObject::connect(HyprSocket::getInstance(), &HyprSocket::hypr_response, gotSignal);
    HyprSocket::getInstance()->sendCommand("cursorpos");
    QObject::disconnect(conn);
    if (!output.isEmpty()){
        output.remove(" ");
        auto new_cursor_pos = QPoint(output.split(",")[0].toInt(), output.split(",")[1].toInt());
        QPoint move_to_cursor_pos = new_cursor_pos - m_dragPosition;
        // qDebug() << output;
        // qDebug() << move_to_cursor_pos;
        QString left_top = QString::number(move_to_cursor_pos.x()) + " " +
                           QString::number(move_to_cursor_pos.y()) + ",title:";

        HyprSocket::getInstance()->sendCommand("dispatch movewindowpixel exact " + left_top + window_title);
        if (show_bar){
            QPoint move_to_bar_pos = QPoint(move_to_cursor_pos.x(), move_to_cursor_pos.y() - 75);

            QString temp_cmd = QString::number(move_to_bar_pos.x()) + " " +
                               QString::number(move_to_bar_pos.y()) + ",title:";

            HyprSocket::getInstance()->sendCommand("dispatch movewindowpixel exact " + temp_cmd +
                                                   "capturepinTool." + window_title.split('.')[1]);
        }
    }


    // QByteArray line;
    // QString output1;
    // qDebug() << process_mouse->readAllStandardOutput();
    // line = process_mouse->readAll();
    // output1 = QString::fromUtf8(line).trimmed();
    // qDebug() << output1;
    // // 如果包含目标内容
    // if (output1.contains("BTN_LEFT")) {
    // qDebug() << "Filtered Line: " << output1;
    //     // process_mouse->terminate();
    //     // process_mouse->waitForFinished();
    //     // delete process_mouse;
    // }
    // QPoint p1 = event->globalPosition().toPoint() - m_dragPosition;
    // p1 = p1 * 1.25;
    // qDebug() << p1;
    // QPoint p2 = p1 + start_move;
    // qDebug() << p2;
    // QProcess process;
    // QString left_top = QString::number(p1.x() * 1.25) + " " + QString::number(p1.y() * 1.25) + "," + "title:";
    // hypr_socket->sendCommand("dispatch movewindowpixel exact " + left_top + window_title);
    // QString output;
    // const auto gotSignal = [&output](QString response) {
    //     output = response;
    // };
    // QMetaObject::Connection conn = QObject::connect(hypr_socket, &HyprSocket::hypr_response, gotSignal);
    // hypr_socket->sendCommand("clients");
    // QObject::disconnect(conn);
    // QStringList lines = output.split("\n");
    // for (int i = 0; i < lines.size(); ++i) {
    //     if (lines[i].contains("title: " + window_title)) {
    //         for (int j = i-1; j >= 0; --j) {
    //             if (lines[j].contains("at: ")) {
    //                 pos = lines[j].split(": ").last().trimmed();
    //                 qDebug() << pos;
    //                 break;
    //             }
    //         }
    //         break;
    //     }
    // }
    // start_move = QPoint(pos.split(",")[0].toInt(), pos.split(",")[1].toInt());

    // QString pid = "pid:" + windowId;
    // QStringList list1;
    // list1 << "-c" << "hyprctl dispatch movewindowpixel exact " + left_top + window_title;
    // process.start("bash", list1);
    // process.waitForStarted();
    // process.waitForFinished();
    // QString output = process.readAll();
    // qDebug() << list1;
    // qDebug() << output;
    // if (m_dragging && (event->buttons() & Qt::LeftButton)) {
    // qDebug() << "real_move";
    // qDebug() << event->globalPosition().toPoint();
    // qDebug() << event->globalPosition().toPoint() - m_dragPosition;
    //     // this->move(event->globalPosition().toPoint() - m_dragPosition);
    // }
    // this->update();
}

void Pin::mouseReleaseEvent(QMouseEvent *event)
{
    // ydotool method:
    if (event->button() == Qt::LeftButton && m_dragging) {
        // qDebug() << "release!";
        QProcess process;
        QStringList list1;
        list1 << "-c" << "ydotool key 125:0 56:0";
        process.startDetached("bash", list1);
        m_dragging = false;
    }
    // end
}

void Pin::scale_pic(int scale_size)
{
    double real_size = (double)scale_size / 10;
    //用于使窗口位置尽量保持不变，但是没必要，而且作用有限
    // QString output;
    // const auto gotSignal = [&output](QString response) {
    //     output = response;
    // };
    // QMetaObject::Connection conn = QObject::connect(hypr_socket, &HyprSocket::hypr_response, gotSignal);
    // hypr_socket->sendCommand("clients");
    // QString old_position;
    // QStringList lines = output.split("\n");
    // for (int i = 0; i < lines.size(); ++i) {
    //     if (lines[i].contains("title: " + window_title)) {
    //         for (int j = i-1; j >= 0; --j) {
    //             if (lines[j].contains("at: ")) {
    //                 old_position = lines[j].split(": ").last().trimmed();
    //                 // qDebug() << pos;
    //                 break;
    //             }
    //         }
    //         break;
    //     }
    // }
    // qDebug() << "Current scale ratio: " << real_size;
    this->label_pic->setPixmap(pic.scaled(pic.size() * real_size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    // this->resize(origin_size * real_size);
    QString resize_param = (QString)QString::number(qRound(origin_size.width() * real_size)) + " " + QString::number(qRound(origin_size.height() * real_size))+ "," + "title:";
    pin_socket->sendCommand("dispatch resizewindowpixel exact " + resize_param + window_title);
    view->setFixedSize(qRound(origin_size.width() * real_size), qRound(origin_size.height() * real_size));
    scene->setSceneRect(0, 0, view->width(), view->height());
    //用于使窗口位置尽量保持不变，但是没必要，而且作用有限
    // hypr_socket->sendCommand("clients");
    // QString new_pos;
    // lines = output.split("\n");
    // for (int i = 0; i < lines.size(); ++i) {
    //     if (lines[i].contains("title: " + window_title)) {
    //         for (int j = i-1; j >= 0; --j) {
    //             if (lines[j].contains("at: ")) {
    //                 new_pos = lines[j].split(": ").last().trimmed();
    //                 // qDebug() << pos;
    //                 break;
    //             }
    //         }
    //         break;
    //     }
    // }
    // QObject::disconnect(conn);
    // if (new_pos != old_position){
    //     QString left_top = old_position.replace(',', ' ') + "," + "title:";
    //     qDebug() << left_top;
    //     hypr_socket->sendCommand("dispatch movewindowpixel exact " + left_top + window_title);
    // }
}

void Pin::wheelEvent(QWheelEvent *event)
{
    int temp = event->angleDelta().y();
    if (temp > 0) {
        if (temp >= 110){
            if (scale_size + 1 <= 100){
                scale_pic(++scale_size);
            }
        }else if (up + temp >= 110){
            if (scale_size + 1 <= 100){
                scale_pic(++scale_size);
            }
            up = 0;
        }else{
            up += temp;
        }
    } else {
        if (temp <= -110){
            if (scale_size - 1 >= 1){
                scale_pic(--scale_size);
            }
        }else if (down + temp <= -110){
            if (scale_size - 1 >= 1){
                scale_pic(--scale_size);
            }
            down = 0;
        }else{
            down += temp;
        }
    }
    // qDebug() << event->angleDelta().y();
}

// bool Pin::event(QEvent *event)
// {
//     const bool ret_val = QWidget::event(event);
//     // qDebug() << event->type();
//     if (event->type() == QEvent::WindowActivate)
//     {
//         if (get_id && !is_hide)
//         {
//             (*paint_count)++;
//             if (*paint_count == 1){
//                 // 窗口显示后执行操作
//                 view->setFixedSize(this->size());
//                 scene->setSceneRect(0, 0, view->width(), view->height());
//                 // QProcess process;
//                 window_title = this->windowTitle(); // Very Important
//                 QString left_top = QString::number(p_start.x()) + " " + QString::number(p_start.y()) + "," + "title:";
//                 // QStringList list1;
//                 // list1 << "-c" << "hyprctl dispatch movewindowpixel exact " + left_top + window_title;
//                 // process.start("bash", list1);
//                 // process.waitForFinished();
//                 this->move(p_start);
//                 HyprSocket::getInstance()->sendCommand("dispatch movewindowpixel exact " + left_top + window_title);
//                 HyprSocket::getInstance()->sendCommand("dispatch setprop title:" + window_title + " norounding 1");
//                 HyprSocket::getInstance()->sendCommand("dispatch setprop title:" + window_title + " noanim 1");
//                 // process.start("bash", QStringList() << "-c" << "hyprctl dispatch setprop title:" + window_title + " norounding 1");
//                 // process.waitForFinished();
//                 this->origin_size = this->size();
//                 get_id = false;
//                 *paint_count = 0;
//             }
//         }
//     }
//     if (event->type() == QEvent::Paint && is_hide)
//     {
//         QString temp_cmd = pos.replace(',', ' ') +  ",title:";
//         // QProcess process;
//         // QStringList list1;
//         // list1 << "-c" << "hyprctl dispatch movewindowpixel exact " + temp_cmd + window_title;
//         // qDebug() << list1;
//         // process.start("bash", list1);
//         // process.waitForFinished();
//         QString output;
//         const auto gotSignal = [&output](QString response) {
//             output = response;
//         };
//         QMetaObject::Connection conn = QObject::connect(HyprSocket::getInstance(),
//                                                         &HyprSocket::hypr_response, gotSignal);
//         HyprSocket::getInstance()->sendCommand("clients");
//         // process.start("bash", QStringList() << "-c" << "hyprctl clients");
//         // process.waitForFinished();
//         // QString output = process.readAll();
//         // 简单解析窗口 ID
//         QObject::disconnect(conn);
//         QStringList lines = output.split("\n");
//         // QString window_seq = "1";
//         for (int i = 0; i < lines.size(); ++i) {
//             if (lines[i].contains(window_title)){
//                 qDebug() << "show!!!";
//                 HyprSocket::getInstance()->sendCommand("dispatch movewindowpixel exact " + temp_cmd + window_title);
//                 HyprSocket::getInstance()->sendCommand("dispatch setprop title:" + window_title + " norounding 1");
//                 HyprSocket::getInstance()->sendCommand("dispatch setprop title:" + window_title + " noanim 1");
//                 if (to_show_bar){
//                     show_toolbar();
//                     to_show_bar = false;
//                 }
//                 // process.start("bash", QStringList() << "-c" << "hyprctl dispatch setprop title:" + window_title + " norounding 1");
//                 // process.waitForFinished();
//                 is_hide = false;
//             }
//         }
//     }
//     return ret_val;
// }

void Pin::showEvent(QShowEvent *event)
{
    // qDebug() << "about_showing";
    // qDebug() << event->type();
    window_title = this->windowTitle(); // Very Important
    QString runtimeDir = qEnvironmentVariable("XDG_RUNTIME_DIR");
    QString socketPath = QString("%1/hypr/%2/.socket2.sock")
                             .arg(runtimeDir, qEnvironmentVariable("HYPRLAND_INSTANCE_SIGNATURE"));
    socket->connectToServer(socketPath);
    // QWidget::showEvent(event);
}

void Pin::hide_all()
{
    QString output;
    HyprSocket* temp_socket = new HyprSocket();
    const auto gotSignal = [&output](QString response) {
        output = response;
    };
    QMetaObject::Connection conn = QObject::connect(temp_socket, &HyprSocket::hypr_response, gotSignal);
    temp_socket->sendCommand("clients");
    // QProcess process;
    // process.start("bash", QStringList() << "-c" << "hyprctl clients");
    // process.waitForFinished();
    // QString output = process.readAll();
    // 简单解析窗口 ID
    QObject::disconnect(conn);
    // qDebug() << output;
    QStringList lines = output.split("\n");
    for (int i = 0; i < lines.size(); ++i) {
        if (lines[i].endsWith("title: " + window_title)) {
            for (int j = i-1; j >= 0; --j) {
                if (lines[j].contains("at: ")) {
                    pos = lines[j].split(": ").last().trimmed();
                    // qDebug() << window_title << pos;
                    break;
                }
            }
            break;
        }
    }
    is_hide =true;
    temp_socket->deleteLater();
    this->hide();
    toolbar->hide();
}

void Pin::show_all()
{
    this->show();
    if (show_bar){
        to_show_bar = true;
    }
}

// void Pin::addText()
// {
//     new_text = !new_text;
//     if (new_text)
//     {
//         view->show();
//         // text_adjust = new QToolBar();
//         // text_adjust->addAction(copyAction);
//         // bar->setGeometry(50, 50, 100, 25);
//         // text_adjust->setGeometry(0, 0, 100, 25);
//         // text_adjust->show();
//     }else{
//         view->hide();
//         // text_adjust->hide();
//         // text_adjust->deleteLater();
//         // text_adjust->show();
//     }
//     qDebug() << "shift+p";
// }

void Pin::show_toolbar()
{
    // bar->show();
    show_bar = true;
    emit show_tool_bar();
}



// void Pin::color_pick()
// {
//     QColor textColor = QColorDialog::getColor();
//     if (selected_textbox){
//         selected_textbox->setDefaultTextColor(textColor);
//     }
// }

void Pin::toggle_toolbar_visible()
{
    if (!show_bar){
        // qDebug() << "showing!";
        show_toolbar();
        // show_bar = true;
        emit restore_toolbar();
        // emit show_tool_bar();
    }else{
        // qDebug() << "unshowing!";
        toolbar->hide();
        show_bar = false;
    }

}

// void Pin::select_textbox(TextBox* textbox)
// {
//     selected_textbox = textbox;
//     qDebug() << textbox->toPlainText();
// }
