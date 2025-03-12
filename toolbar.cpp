#include "toolbar.h"
#include "pin.h"
// #include "hyprsocket.h"
// #include "ui_toolbar.h"
#include "lline.h"
#include "colorgridwidget.h"
#include "sharevar.h"
#include "textbox.h"
#include <QColorDialog>
#include <QProcess>

ToolBar::ToolBar(QGraphicsScene *scene, QString window_title, QWidget *parent)
    : parent(parent), default_font_size(ShareVar::default_font_size)
// , ui(new Ui::ToolBar)
{
    // ui->setupUi(this);
    this->scene = scene;
    this->view = static_cast<GraphView*>(scene->views().constFirst());
    this->window_title = window_title;
    socket = new QLocalSocket();
    connect(socket, &QLocalSocket::readyRead, this, &ToolBar::receive_socket);
    toolbar_socket = new HyprSocket(this);
    addText = new QAction("T", this);
    addLine = new QAction("/", this);
    complete = new QAction("done", this);
    addText->setCheckable(true);
    addLine->setCheckable(true);
    // qDebug() << this->window_title;
    toolbar = new QToolBar(this);
    toolbar->addAction(addText);
    toolbar->addAction(addLine);
    toolbar->addSeparator();
    toolbar->addAction(complete);
    toolbar->show();
    connect(scene, &QGraphicsScene::focusItemChanged, this, &ToolBar::get_focus_item);
    connect(addText, &QAction::triggered, this, &ToolBar::addText_button_clicked);
    connect(addLine, &QAction::triggered, this, &ToolBar::addLine_button_clicked);
    connect(complete, &QAction::triggered, this, &ToolBar::edit_done);
    key_toggle_bar = new QShortcut(QKeySequence(myset.value("toggle_toolbar_visible").toString()), this);
    auto del_item = new QAction(this);
    del_item->setShortcut(QKeySequence("Delete"));
    this->addAction(del_item);
    this->parent->addAction(del_item);
    connect(del_item, &QAction::triggered, this, &ToolBar::delete_focus_item);
    connect(key_toggle_bar, &QShortcut::activated, this, &ToolBar::toggle_toolbar_visible);
    connect(this, &ToolBar::set_view_cursor, view, &GraphView::set_cursor);
    connect(this, &ToolBar::set_view_tool, view, &GraphView::set_tool);
    current_tool = Tools::NONE;
    // text_adjust = new QToolBar(this);

}

ToolBar::~ToolBar()
{
    // delete ui;
    qDebug() << "toolbar close!";
}

void ToolBar::addText_button_clicked(bool checked)
{
    if (checked)
    {
        if (text_adjust == nullptr){
            text_adjust = new QToolBar(this);
            text_adjust->setGeometry(0, 25, 1000, 40);
            QList<QColor> colors = {
                Qt::black, Qt::gray, Qt::red, Qt::yellow, Qt::green, Qt::blue, Qt::white,
                Qt::darkRed, Qt::darkYellow, Qt::darkGreen, Qt::darkBlue, Qt::cyan, Qt::magenta
            };
            ColorGridWidget* color_picker = new ColorGridWidget(colors, text_adjust);
            // text_color = new QAction("Color");
            // connect(text_color, &QAction::triggered, this, &ToolBar::change_text_color);
            text_color_button = new QPushButton(text_adjust);
            // text_color_button->setObjectName("text_color");
            text_color_button->setStyleSheet("background-color: #3498DB; border-radius: 5px;");
            textbox_color = QColor::fromRgb(52, 152, 219); // #3498DB
            // color_button->setGeometry(color_button->pos().x(), color_button->pos().y(), 5, 5);
            // color_button->resize(1, 1);
            text_color_button->setFixedSize(30, 30);
            connect(text_color_button, &QPushButton::clicked, this, &ToolBar::color_dialog);
            bold_text = new QAction("B", text_adjust);
            bold_text->setShortcut(QKeySequence("Ctrl+B"));
            this->parent->addAction(bold_text);
            italic_text = new QAction("I", text_adjust);
            italic_text->setShortcut(QKeySequence("Ctrl+I"));
            this->parent->addAction(italic_text);
            bold_text->setCheckable(true);
            italic_text->setCheckable(true);
            text_adjust->addWidget(color_picker);
            text_adjust->addWidget(text_color_button);
            // text_adjust->addAction(text_color);
            text_adjust->addAction(bold_text);
            text_adjust->addAction(italic_text);
            text_font = new QFontComboBox(text_adjust);
            text_font->setSizeAdjustPolicy(QComboBox::AdjustToContents);
            QFont font("Noto Sans Mono");
            text_font->setCurrentFont(font);
            text_size = new QComboBox(text_adjust);
            text_size->setEditable(true);
            // text_size->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
            text_size->setMinimumContentsLength(4);
            text_size->setInsertPolicy(QComboBox::NoInsert);
            auto valid_size = new QDoubleValidator(text_adjust);
            valid_size->setRange(1.0, 100.0, 1);
            valid_size->setNotation(QDoubleValidator::StandardNotation);
            text_size->setValidator(valid_size);
            QStringList text_size_option;
            for (int i = 1; i <= 100; i++){
                text_size_option.append(QString::number(i));
            }
            text_size->addItems(text_size_option);
            text_size->setCurrentText(QString::number(default_font_size));
            text_adjust->addWidget(text_size);
            text_adjust->addWidget(text_font);
            connect(color_picker, &ColorGridWidget::colorSelected, this, &ToolBar::get_selected_color);
            connect(text_size, &QComboBox::editTextChanged, this, &ToolBar::change_text_size);
            connect(text_font, &QFontComboBox::currentFontChanged, this, &ToolBar::change_text_font);
            connect(bold_text, &QAction::triggered, this, &ToolBar::bold_text_button_clicked);
            connect(italic_text, &QAction::triggered, this, &ToolBar::italic_text_button_clicked);
        }
        emit view_visible(true);
        emit set_view_cursor(QCursor(Qt::IBeamCursor));
        this->setCursor(Qt::IBeamCursor);
        this->setCursor(Qt::ArrowCursor);
        reset_last_tool();
        current_tool = Tools::TEXT;
        text_adjust->show();
        const auto &items = this->scene->items();
        for (const auto &item : items) {
            if (item->type() == TextBox::Type){
                item->setEnabled(true);
            }
        }
    }else{
        current_tool = Tools::NONE;
        const auto &items = this->scene->items();
        for (const auto &item : items) {
            if (item->type() == TextBox::Type){
                item->setZValue(0);
                auto textbox = static_cast<TextBox*>(item);
                textbox->is_hovering = false;
                item->setEnabled(false);
            }
        }
        emit set_view_cursor(QCursor(Qt::ArrowCursor));
        if (text_adjust != nullptr){
            text_adjust->hide();
        }
    }
}

void ToolBar::addLine_button_clicked(bool checked)
{
    if (checked)
    {
        if (line_tool == nullptr){
            line_tool = new QToolBar(this);
            line_tool->setGeometry(0, 25, 1000, 40);
            QList<QColor> colors = {
                Qt::black, Qt::gray, Qt::red, Qt::yellow, Qt::green, Qt::blue, Qt::white,
                Qt::darkRed, Qt::darkYellow, Qt::darkGreen, Qt::darkBlue, Qt::cyan, Qt::magenta
            };
            ColorGridWidget* color_picker = new ColorGridWidget(colors, line_tool);
            line_width_button = new PenSizeButton(line_tool);
            line_color_button = new QPushButton(line_tool);
            // line_color_button->setObjectName("line_color");
            line_color_button->setStyleSheet("background-color: #3498DB; border-radius: 5px;");
            line_color = QColor::fromRgb(52, 152, 219); // #3498DB
            line_color_button->setFixedSize(30, 30);
            connect(line_color_button, &QPushButton::clicked, this, &ToolBar::color_dialog);
            line_tool->addWidget(color_picker);
            line_tool->addWidget(line_color_button);
            line_tool->addWidget(line_width_button);
            connect(color_picker, &ColorGridWidget::colorSelected, this, &ToolBar::get_selected_color);
            // connect(line_width_button, &PenSizeButton::penSizeChanged, this, &ToolBar::set_color_cursor);
        }
        emit view_visible(true);
        set_color_cursor(line_color, line_width_button->penSize());
        reset_last_tool();
        current_tool = Tools::LINE;
        const auto &items = this->scene->items();
        for (const auto &item : items) {
            if (item->type() == Lline::Type){
                item->setEnabled(true);
            }
        }
        line_tool->show();
    }else{
        current_tool = Tools::NONE;
        emit set_view_cursor(Qt::ArrowCursor);
        const auto &items = this->scene->items();
        for (const auto &item : items) {
            if (item->type() == Lline::Type){
                item->setZValue(0);
                item->setEnabled(false);
            }
        }
        if (line_tool != nullptr){
            line_tool->hide();
        }
    }
}

void ToolBar::eraser_button_clicked(bool checked)
{
    if (checked)
    {
        if (eraser_tool == nullptr){
            eraser_tool = new QToolBar(this);
            eraser_width_button = new PenSizeButton(eraser_tool);
            eraser_tool->addWidget(eraser_width_button);
            // connect(eraser_width_button, &PenSizeButton::penSizeChanged, this, &ToolBar::set_color_cursor);
        }
        emit view_visible(true);
        set_color_cursor(Qt::white, 5, true);
        reset_last_tool();
        current_tool = Tools::ERASER;
        const auto &items = this->scene->items();
        for (const auto &item : items) {
            if (item->type() == Lline::Type){
                item->setEnabled(true);
            }
        }
        eraser_tool->show();
    }else{
        current_tool = Tools::NONE;
        emit set_view_cursor(Qt::ArrowCursor);
        const auto &items = this->scene->items();
        for (const auto &item : items) {
            if (item->type() == Lline::Type){
                item->setZValue(0);
                item->setEnabled(false);
            }
        }
        if (eraser_tool != nullptr){
            eraser_tool->hide();
        }
    }
}

void ToolBar::bold_text_button_clicked(bool checked)
{
    // qDebug() << "bold_text clicked!";
    if (scene->focusItem() != nullptr){
        if (scene->focusItem()->type() == TextBox::Type){
            auto text_box = static_cast<TextBox*>(scene->focusItem());
            QFont temp_font = text_box->font();
            temp_font.setBold(checked);
            text_box->setFont(temp_font);
        }
    }
}

void ToolBar::italic_text_button_clicked(bool checked)
{
    if (scene->focusItem() != nullptr){
        if (scene->focusItem()->type() == TextBox::Type){
            auto text_box = static_cast<TextBox*>(scene->focusItem());
            QFont temp_font = text_box->font();
            temp_font.setItalic(checked);
            text_box->setFont(temp_font);
        }
    }
}

void ToolBar::color_dialog()
{
    QColor color = QColorDialog::getColor();
    if (color.isValid()){
        update_item_color(color);
    }
}

void ToolBar::edit_done()
{
    emit view_visible(false);
    switch (current_tool) {
    case Tools::TEXT:
        addText->setChecked(false);
        addText_button_clicked(false);
        text_adjust->hide();
        text_adjust->deleteLater();
        break;
    case Tools::LINE:
        addLine->setChecked(false);
        addLine_button_clicked(false);
        line_tool->hide();
        line_tool->deleteLater();
        break;
    default:
        break;
    }
    current_tool = Tools::NONE;
    QPixmap pixmap(static_cast<Pin*>(this->parent)->get_pic());
    pixmap.setDevicePixelRatio(1);
    // qDebug() << pixmap.size();
    // pixmap.fill(Qt::transparent);
    // qDebug() << "start render";
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    scene->render(&painter);
    // qDebug() << "render finished";
    painter.end();
    pixmap.setDevicePixelRatio(ShareVar::device_pixel_ratio);
    emit update_pic(pixmap);
    this->hide();
}

void ToolBar::delete_focus_item()
{
    if (this->scene->focusItem()){
        delete this->scene->focusItem();
    }
}

void ToolBar::change_text_size(const QString &size)
{
    if (scene->focusItem() != nullptr){
        switch (scene->focusItem()->type()) {
        case TextBox::Type:{
            auto item = static_cast<TextBox*>(scene->focusItem());
            item->setScale(size.toDouble() / default_font_size);
            break;
        }
        default:
            break;
        }
        // qDebug() << "text size changed!";
    }
}

void ToolBar::change_text_font(const QFont &font)
{
    if (scene->focusItem() != nullptr && scene->focusItem()->type() == TextBox::Type){
        auto item = static_cast<TextBox*>(scene->focusItem());
        QFont new_font = item->font();
        new_font.setFamilies({font.family(), "Noto Color Emoji"});
        item->setFont(new_font);
        // qDebug() << "text font changed!";
    }
}

void ToolBar::window_shown()
{
    if (to_show_bar){
        QString pos;
        QString output;
        QProcess process;
        // find this window
        process.start("bash", QStringList() << "-c" << "hyprctl clients");
        process.waitForFinished();
        output = process.readAll();
        // const auto gotSignal = [&output](QString response) {
        //     output = response;
        // };
        // QMetaObject::Connection conn = QObject::connect(toolbar_socket,
        //                                                 &HyprSocket::hypr_response, gotSignal);
        // toolbar_socket->sendCommand("clients");
        //
        // QObject::disconnect(conn);
        QStringList lines = output.split("\n");
        for (int i = 0; i < lines.size(); ++i) {
            if (lines[i].endsWith(QString("title: ") + this->windowTitle())){
                // qDebug() << "show";
                for (int i = 0; i < lines.size(); ++i) {
                    // find the pin window position
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
                pos = pos.split(",")[0] + " " + QString::number(pos.split(",")[1].toInt() - 75);
                QString temp_cmd = pos.replace(',', ' ') +  ",title:";
                // process.startDetached("bash", QStringList() << "-c" << QString("hyprctl dispatch setfloating title:") + this->windowTitle());
                // HyprSocket temp_socket;
                // temp_socket.sendCommand("dispatch movewindowpixel exact " + temp_cmd + this->windowTitle());
                process.startDetached("bash", QStringList() << "-c" << "hyprctl dispatch 'movewindowpixel exact " + temp_cmd + this->windowTitle() + "'");
                process.startDetached("bash", QStringList() << "-c" << QString("hyprctl dispatch setprop title:") + this->windowTitle() + " norounding 1");
                // toolbar_socket->sendCommand("dispatch movewindowpixel exact " +
                //                             temp_cmd + this->windowTitle());
                if (reshow_bar){
                    // toolbar_socket->sendCommand("dispatch focuswindow title:" + window_title);
                    // this->setFocus();
                    reshow_bar = false;
                }
                // toolbar_socket->sendCommand(QString("dispatch setprop title:") + this->windowTitle() + " norounding 1");
                to_show_bar = false;
            }
        }
    }
}

void ToolBar::update_item_color(const QColor &color)
{
    switch (current_tool) {
    case Tools::TEXT:
        text_color_button->setStyleSheet("background-color: " + color.name() + "; border-radius: 5px;");
        textbox_color = color;
        break;
    case Tools::LINE:
        line_color_button->setStyleSheet("background-color: " + color.name() + "; border-radius: 5px;");
        line_color = color;
        set_color_cursor(line_color, line_width_button->penSize());
        break;
    default:
        break;
    }
    if (scene->focusItem() != nullptr){
        switch (scene->focusItem()->type()) {
        case TextBox::Type:
            // if (current_tool == TEXTTOOL)
            {
                auto text_box = static_cast<TextBox*>(scene->focusItem());
                text_box->setDefaultTextColor(color);
            }
            break;
        case Lline::Type:
            // if (current_tool == LINE)
            {
                auto line = static_cast<Lline*>(scene->focusItem());
                QPen pen(line->pen());
                pen.setColor(color);
                line->setPen(pen);
            }
            break;
        default:
            break;
        }
    }
}

void ToolBar::reset_last_tool()
{
    switch (current_tool) {
    case Tools::TEXT:
        addText->setChecked(false);
        addText_button_clicked(false);
        break;
    case Tools::LINE:
        addLine->setChecked(false);
        addLine_button_clicked(false);
        break;
    default:
        break;
    }
}

void ToolBar::set_color_cursor(const QColor &color, int size, bool bound)
{
    QPixmap cursorPixmap(size, size);
    cursorPixmap.fill(Qt::transparent);
    QPainter painter(&cursorPixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    if (bound){
        QPen pen;
        pen.setWidth(0);
        pen.setColor(Qt::black);
        painter.setPen(pen);
    }else {
        painter.setPen(Qt::NoPen);
    }
    painter.setBrush(QBrush(color));
    double r = size / 2.0;
    painter.drawEllipse(QPointF(r, r), r, r);
    emit set_view_cursor(QCursor(cursorPixmap, r, r));
    this->setCursor(Qt::IBeamCursor);
    this->setCursor(Qt::ArrowCursor);
}

void ToolBar::receive_socket()
{
    QString event = socket->readAll();
    QStringList events = event.split('\n');
    foreach (QString i, events) {
        if (i.contains("openwindow") && i.endsWith(this->windowTitle())){
            socket->disconnectFromServer();
            window_shown();
        }
    }
}

void ToolBar::closeEvent(QCloseEvent *event)
{
    emit toggle_toolbar_visible();
    QWidget::closeEvent(event);
}

void ToolBar::show_toolbar()
{
    to_show_bar = true;
    this->show();
}

void ToolBar::restore_toolbar()
{
    this->reshow_bar = true;
}

void ToolBar::get_focus_item(QGraphicsItem *newFocusItem, QGraphicsItem *oldFocusItem, Qt::FocusReason reason)
{
    // qDebug() << newFocusItem << oldFocusItem;
    if (!newFocusItem){
        return;
    }
    switch (newFocusItem->type()) {
    case TextBox::Type:
        if (text_adjust){
            auto textbox = static_cast<TextBox*>(newFocusItem);
            QFont temp_font;
            temp_font.setFamily(textbox->font().family());
            const QSignalBlocker blocker(text_font);
            const QSignalBlocker blocker1(text_size);
            const QSignalBlocker blocker2(bold_text);
            const QSignalBlocker blocker3(italic_text);
            text_font->setCurrentFont(temp_font);
            text_size->setCurrentText(QString::number(qRound(default_font_size * 10 * textbox->scale()) / 10.0));
            bold_text->setChecked(textbox->font().bold());
            italic_text->setChecked(textbox->font().italic());
            text_color_button->setStyleSheet("background-color: " + textbox->defaultTextColor().name() + "; border-radius: 5px;");
            break;
        }
    case Lline::Type:
        if (line_tool){
            auto line = static_cast<Lline*>(newFocusItem);
            line_color_button->setStyleSheet("background-color: " + line->pen().color().name() + "; border-radius: 5px;");
        }
    default:
        break;
    }
    // set item stack order
    if (newFocusItem != oldFocusItem){
        newFocusItem->setZValue(1);
        if (oldFocusItem){
            oldFocusItem->setZValue(0);
            if (oldFocusItem->type() == TextBox::Type){
                auto item = static_cast<TextBox*>(oldFocusItem);
                if (item->can_delete){
                    item->deleteLater();
                }
            }
        }

    }
}

void ToolBar::get_selected_color(const QColor &color)
{
    update_item_color(color);
}

void ToolBar::screen_clicked(QMouseEvent *event)
{
    // qDebug() << "not_hovering";
    QGraphicsItem* item = scene->itemAt(event->scenePosition(), view->transform());
    switch (current_tool) {
    case Tools::TEXT:
        // QGraphicsItem* item = scene->itemAt(event->scenePosition(), view->transform());
        if (!item || item->type() != TextBox::Type){
            // qDebug() << "add new textbox!";
            TextBox* text_box_1 = new TextBox();
            // connect(text_box_1, &TextBox::select_textbox, this, &GraphView::current_object);
            // text_box_1->setTextWidth(10);
            // text_box_1->setPlainText("可调整大小和旋转的文本框");
            // QFont font("Noto Sans Mono");
            // font.setStyleHint(QFont::Monospace);
            // font.setFamilies({"Noto Sans Mono", "Noto Color Emoji"});
            QFont font = text_font->currentFont();
            font.setFamilies({font.family(), "Noto Color Emoji"});
            font.setPointSize(default_font_size);
            text_box_1->setFont(font);
            if (textbox_color.isValid()){
                text_box_1->setDefaultTextColor(textbox_color);
            }

            // qDebug() << event->scenePosition();
            // qDebug() << text_box_1->scenePos();
            text_box_1->setScale(text_size->currentText().toDouble() / default_font_size);
            text_box_1->setTransformOriginPoint(text_box_1->boundingRect().center());
            text_box_1->setPos(event->scenePosition() - text_box_1->boundingRect().center());
            // qDebug() << "add_text";
            scene->addItem(text_box_1);
            // qDebug() << text_box_1->isVisible();
            // connect(text_box_1, &TextBox::select_textbox, this, &ToolBar::get_selected_textbox);
            connect(text_box_1, &TextBox::update_font_size, this, &ToolBar::update_font_size);
            scene->update();
        }
        break;
    case Tools::LINE:
        if (!item || item->type() != Lline::Type){
            auto line = new Lline(this);
            line->setLine(QLineF(0, 0, 0, 0.1));
            line->setPos(event->scenePosition());
            // line->setPos(line->mapToScene(line->mapFromScene(event->scenePosition()) - QPointF(0, 1)));
            qDebug() << event->scenePosition();
            qDebug() << line->mapToScene(line->line().p1()) << line->mapToScene(line->line().p2());
            QPen pen;
            pen.setWidth(line_width_button->penSize());
            pen.setColor(line_color);
            pen.setCapStyle(Qt::RoundCap);
            line->setPen(pen);
            scene->addItem(line);
        }
    default:
        break;
    }
}

void ToolBar::update_font_size(double size)
{
    const QSignalBlocker blocker(text_size);
    text_size->setCurrentText(QString::number(size));
}

void ToolBar::showEvent(QShowEvent *event)
{
    QString runtimeDir = qEnvironmentVariable("XDG_RUNTIME_DIR");
    QString socketPath = QString("%1/hypr/%2/.socket2.sock")
                             .arg(runtimeDir, qEnvironmentVariable("HYPRLAND_INSTANCE_SIGNATURE"));
    socket->connectToServer(socketPath);
}
