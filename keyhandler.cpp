#include "keyhandler.h"
// #include <QtWidgets>

KeyHandler KeyHandler::instance;

KeyHandler& KeyHandler::getInstance()
{
    return instance;
}

KeyHandler::KeyHandler(QObject *parent)
    : QObject{parent}
{
    // myset = new QSettings("CapturePin", "Config");
}

// void KeyHandler::start_process(QString window_title)
// {
//     if (live_pin.empty()){
//         process_mouse = new QProcess();
//         connect(process_mouse, &QProcess::readyReadStandardOutput, this, &KeyHandler::detect_mouse_status);
//         process_mouse->start("bash", QStringList() << "-c" << "libinput debug-events");
//     }
//     live_pin.append(window_title);
// }

// void KeyHandler::process_key()
// {
//     m_dragging = true;
// }

// void KeyHandler::close_process(QString window_title)
// {
//     live_pin.removeOne(window_title);
//     // qDebug() << live_pin;
//     if (live_pin.empty()){
//         // qDebug() << "live_pin is empty!";
//         process_mouse->terminate();
//         process_mouse->waitForFinished();
//         disconnect(process_mouse, &QProcess::readyReadStandardOutput, this, &KeyHandler::detect_mouse_status);
//         process_mouse->deleteLater();
//         // if (process_mouse->state() == QProcess::NotRunning)
//         // {
//         //     qDebug() << process_mouse->state();
//         // }
//     }
// }

// void KeyHandler::detect_mouse_status()
// {
//     // qDebug() << process_mouse->readLine();
//     QByteArray line;
//     QString output1;
//     // qDebug() << process_mouse->readAll();
//     line = process_mouse->readAll();
//     output1 = QString::fromUtf8(line).trimmed();
//     // 如果包含目标内容
//     if (m_dragging && output1.contains("BTN_LEFT")  && output1.contains("released")) {
//         // qDebug() << "Filtered Line: " << output1;
//         m_dragging = false;
//         QProcess process;
//         QStringList list1;
//         list1 << "-c" << "ydotool click 0xc0";
//         process.startDetached("bash", list1);
//         QStringList list2;
//         list2 << "-c" << "ydotool key 125:0 56:0";
//         process.startDetached("bash", list2);
//     }
//     else if (output1.contains("POINTER_MOTION") && show_toolbar && m_dragging){
//         QString pos2 = "";
//         QString pos1;
//         QString window_title = "PinnedScreenshot.1";
//         QString output;
//         const auto gotSignal = [&output](QString response) {
//             output = response;
//         };
//         QMetaObject::Connection conn = QObject::connect(hypr_socket, &HyprSocket::hypr_response, gotSignal);
//         hypr_socket->sendCommand("clients");
//         QObject::disconnect(conn);
//         // QProcess process;
//         // process.start("bash", QStringList() << "-c" << "hyprctl clients");
//         // process.waitForFinished();
//         // QString output = process.readAll();
//         QStringList lines = output.split("\n");
//         // QString window_seq = "1";
//         for (int i = 0; i < lines.size(); ++i) {
//             if (lines[i].contains("title: " + window_title)) {
//                 for (int j = i-1; j >= 0; --j) {
//                     if (lines[j].contains("at: ")) {
//                         pos1 = lines[j].split(": ").last().trimmed();
//                         break;
//                     }
//                 }
//                 break;
//             }
//         }
//         if (pos1 != pos2)
//         {
//             if (pos1.split(",")[1].toInt() - 55 < 0){
//                 pos1 = pos1.split(",")[0] + ",55";
//             }
//             pos2 = pos1;
//             pos1 = pos1.split(",")[0] + " " + QString::number(pos1.split(",")[1].toInt() - 55);
//             QString temp_cmd = pos1 +  ",title:";
//             hypr_socket->sendCommand("dispatch movewindowpixel exact " + temp_cmd + "capturepinTool.1");
//             // process.startDetached("bash", QStringList() << "-c" << "hyprctl dispatch movewindowpixel exact " + temp_cmd + "capturepinTool.1");
//             // process.waitForFinished();
//         }
//     }

//     // if (myset->value("enableShortcut") == 1 && output1.contains("BTN_LEFT")  && output1.contains("released")){
//     //     qDebug() << "enabled!";
//     // }
// }

void KeyHandler::show_hide_all()
{
    if (is_show){
        emit hide_all();
        is_show = false;
    }else{
        emit show_all();
        is_show = true;
    }
}

void KeyHandler::show_tool_bar()
{
    show_toolbar = true;
}
