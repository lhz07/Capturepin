#include "pin.h"
#include "start_shot.h"
// #include <QtWidgets>
#include <unistd.h>
#include "hyprsocket.h"
#include <QMenu>
#include <QGuiApplication>
#include <QClipboard>
#include <QDateTime>
#include <QFileDialog>
#include <QMouseEvent>
#include <QWindow>

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
    mainLayout->addWidget(label_pic, 0);
    label_pic->lower();
    copy_pic = new QShortcut(QKeySequence("Ctrl+C"), this);
    key_show_bar = new QShortcut(QKeySequence("Space"), this);
    myset.setValue("toggle_toolbar_visible", "Space");
    connect(copy_pic, &QShortcut::activated, this, &Pin::copy);
    connect(key_show_bar, &QShortcut::activated, this, &Pin::toggle_toolbar_visible);
    menu = new QMenu(this);
    quickSaveAction = new QAction("Quick save", this);
    saveAction = new QAction("Save picture", this);
    copyAction = new QAction("Copy picture", this);
    closeAction = new QAction("Close", this);
    toolbarAction = new QAction("Show/Hide toolbar", this);
    connect(quickSaveAction, &QAction::triggered, this, &Pin::quick_save);
    connect(saveAction, &QAction::triggered, this, &Pin::save_pic);
    connect(copyAction, &QAction::triggered, this, &Pin::copy);
    connect(closeAction, &QAction::triggered, this, &QWidget::close);

    connect(closeAction, &QAction::triggered, toolbar, &QWidget::close);
    connect(toolbarAction, &QAction::triggered, this, &Pin::toggle_toolbar_visible);
    menu->addAction(copyAction);
    menu->addAction(toolbarAction);
    menu->addAction(quickSaveAction);
    menu->addAction(saveAction);
    menu->addAction(closeAction);
    QString w_h = QString::number(pic.size().width()) + "×" + QString::number(pic.size().height());
    menu->addAction(w_h);

    connect(view, &GraphView::mouse_clicked, toolbar, &ToolBar::screen_clicked);
    connect(toolbar, &ToolBar::view_visible, view, &GraphView::text_tool_visible);
    // view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setFrameShape(QFrame::NoFrame);
    view->hide();
    // view->setGeometry(0, 0, this->size().width(), this->size().height());

    view->setStyleSheet("background: transparent;");
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
    const QStringList events = event.split('\n');
    for (const QString &i : events) {
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
            // 窗口显示后执行操作
            view->setFixedSize(this->size());
            scene->setSceneRect(0, 0, view->width(), view->height());
            QString left_top = QString::number(p_start.x()) + " " + QString::number(p_start.y()) + "," + "title:";
            this->move(p_start);
            // qDebug() << "move to" << p_start;
            pin_socket->sendCommand("dispatch movewindowpixel exact " + left_top + window_title);
            pin_socket->sendCommand("dispatch setprop title:" + window_title + " norounding 1");
            this->origin_size = this->size();
            get_id = false;
    }
    else if (is_hide)
    {
        QString temp_cmd = pos.replace(',', ' ') +  ",title:";
        QProcess process;
        process.startDetached("bash", QStringList() << "-c" << "hyprctl dispatch movewindowpixel exact " + temp_cmd + window_title);
        process.startDetached("bash", QStringList() << "-c" << "hyprctl dispatch setprop title:" + window_title + " norounding 1");
        if (to_show_bar){
            show_toolbar();
            to_show_bar = false;
        }
        is_hide = false;
    }
}

void Pin::save_pic()
{
    QString name = QString("Capturepin_%1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss"));
    QFileDialog dialog(this, "Save Picture", QDir::homePath()+'/'+name, "PNG Files (*.png);;JPEG Files (*.jpeg)");
    QString file_path;
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() == QDialog::Accepted) {
        // get file path
        QStringList selectedFiles = dialog.selectedFiles();
        if (!selectedFiles.empty()){
            file_path = selectedFiles.constFirst();
        }
        // get file extension
        QString selectedFilter = dialog.selectedNameFilter();
        // qDebug() << file_path << selectedFilter;
        if (file_path != ""){
            if (selectedFilter == "PNG Files (*.png)"){
                if (!file_path.endsWith(".png")) {
                    file_path += ".png";
                }
                pic.save(file_path, "png");
            }
            else if (selectedFilter == "JPEG Files (*.jpeg)"){
                if (!file_path.endsWith(".jpeg")) {
                    file_path += ".jpeg";
                }
                pic.save(file_path, "jpeg", 100);
            }
        }
    }
}


void Pin::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (myset.value("DE") == "Hyprland"){
            m_dragging = true;
            QString output;
            const auto gotSignal = [&output](QString response) {
                output = response;
            };
            pin_socket->sendCommand("dispatch setprop title:" + window_title + " noanim 1");
            QMetaObject::Connection conn = QObject::connect(pin_socket, &HyprSocket::hypr_response, gotSignal);
            pin_socket->sendCommand("clients");
            QStringList lines = output.split("\n");
            for (int i = 0; i < lines.size(); ++i) {
                if (lines[i].endsWith("title: " + window_title)) {
                    for (int j = i-1; j >= 0; --j) {
                        if (lines[j].contains("at: ")) {
                            pos = lines[j].split(": ").last().trimmed();
                            // qDebug() << pos;
                            break;
                        }
                    }
                    break;
                }
            }
            start_move = QPoint(pos.split(",")[0].toInt(), pos.split(",")[1].toInt());
            pin_socket->sendCommand("cursorpos");
            QObject::disconnect(conn);
            qDebug() << "pin press!";
            // qDebug() << view->rect() << view->isVisible();
            output.remove(" ");
            auto old_cursor_pos = QPoint(output.split(",")[0].toInt(), output.split(",")[1].toInt());
            m_dragPosition = old_cursor_pos - start_move;
            // qDebug() << start_move;
            // qDebug() << m_dragPosition;
            if (show_bar){
                pin_socket->sendCommand("dispatch setprop title:capturepinTool." + window_title.split('.')[1] + " noanim 1");
            }
        }
        else {
            QWindow *window = this->windowHandle();
            // m_dragPosition = event->globalPosition().toPoint() - window->position();
            if (window) {
                bool result = window->startSystemMove(); // 触发系统移动
                qDebug() << "Start system move triggered:" << result;
            }
        }
    }
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
}

void Pin::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_dragging) {
        pin_socket->sendCommand("dispatch setprop title:" + window_title + " noanim 0");
        if (show_bar){
            pin_socket->sendCommand("dispatch setprop title:capturepinTool." + window_title.split('.')[1] + " noanim 0");
        }
        m_dragging = false;
    }
}

void Pin::scale_pic(int scale_size)
{
    double real_size = (double)scale_size / 10;
    this->label_pic->setPixmap(pic.scaled(pic.size() * real_size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    // this->resize(origin_size * real_size);
    QString resize_param = (QString)QString::number(qRound(origin_size.width() * real_size)) + " " + QString::number(qRound(origin_size.height() * real_size))+ "," + "title:";
    pin_socket->sendCommand("dispatch resizewindowpixel exact " + resize_param + window_title);
    view->setFixedSize(qRound(origin_size.width() * real_size), qRound(origin_size.height() * real_size));
    scene->setSceneRect(0, 0, view->width(), view->height());
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
//     qDebug() << event->type();
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
    QObject::disconnect(conn);
    // qDebug() << output;
    // find window position
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

void Pin::show_toolbar()
{
    // bar->show();
    show_bar = true;
    emit show_tool_bar();
}

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
