#include "toolbar.h"
// #include "hyprsocket.h"
enum tools{NONE, TEXTTOOL, RECTANGLE};
// #include "ui_toolbar.h"
#include "colorgridwidget.h"

ToolBar::ToolBar(QGraphicsScene *scene, QString window_title, QWidget *parent)
    : QWidget(parent)
// , ui(new Ui::ToolBar)
{
    // ui->setupUi(this);
    this->scene = scene;
    this->view = static_cast<GraphView*>(scene->views()[0]);
    this->window_title = window_title;
    toolbar_socket = new HyprSocket(this);
    addText = new QAction("T");
    complete = new QAction("done");
    addText->setCheckable(true);
    qDebug() << this->window_title;
    toolbar = new QToolBar(this);
    toolbar->addAction(addText);
    toolbar->addSeparator();
    toolbar->addAction(complete);
    toolbar->show();
    connect(scene, &QGraphicsScene::focusItemChanged, this, &ToolBar::get_focus_item);
    connect(addText, &QAction::triggered, this, &ToolBar::addText_button_clicked);
    connect(complete, &QAction::triggered, this, &ToolBar::edit_done);
    text_color = new QAction("Color");
    connect(text_color, &QAction::triggered, this, &ToolBar::change_text_color);
    key_toggle_bar = new QShortcut(QKeySequence(myset.value("toggle_toolbar_visible").toString()), this);
    connect(key_toggle_bar, &QShortcut::activated, this, &ToolBar::toggle_toolbar_visible);
    current_tool = NONE;
    // text_adjust = new QToolBar(this);

}

ToolBar::~ToolBar()
{
    // delete ui;
    qDebug() << "close!";
}

void ToolBar::addText_button_clicked()
{
    if (addText->isChecked())
    {
        if (text_adjust == nullptr){
            text_adjust = new QToolBar(this);
            text_adjust->setGeometry(0, 25, 1000, 40);
            QList<QColor> colors = {
                Qt::black, Qt::gray, Qt::red, Qt::yellow, Qt::green, Qt::blue, Qt::white,
                Qt::darkRed, Qt::darkYellow, Qt::darkGreen, Qt::darkBlue, Qt::cyan, Qt::magenta
            };
            ColorGridWidget* color_picker = new ColorGridWidget(colors, text_adjust);
            text_adjust->addWidget(color_picker);
            text_adjust->addAction(text_color);
            text_font = new QFontComboBox(text_adjust);
            QFont font("Noto Sans Mono");
            font.setStyleHint(QFont::Monospace);
            font.setFamilies({"Noto Sans Mono", "Noto Color Emoji"});
            text_font->setCurrentFont(font);
            text_size = new QComboBox(text_adjust);
            text_size->setEditable(true);
            text_size->setSizeAdjustPolicy(QComboBox::AdjustToContents);
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
            text_size->setCurrentText("12");
            text_adjust->addWidget(text_size);
            text_adjust->addWidget(text_font);
            connect(color_picker, &ColorGridWidget::colorSelected, this, &ToolBar::get_selected_color);
            connect(text_size, &QComboBox::editTextChanged, this, &ToolBar::change_text_size);
            connect(text_font, &QFontComboBox::currentFontChanged, this, &ToolBar::change_text_font);
        }
        emit view_visible(true);
        current_tool = TEXTTOOL;
        text_adjust->show();
    }else{
        current_tool = NONE;
        if (text_adjust != nullptr){
            text_adjust->hide();
        }
    }
}

void ToolBar::change_text_color()
{
    QColor textColor = QColorDialog::getColor();
    // if (!selected_textbox.isNull()){
    //     selected_textbox->setDefaultTextColor(textColor);
    // }
    if (scene->focusItem() != nullptr){
        if (scene->focusItem()->type() == TextBox::Type){
            auto text_box = static_cast<TextBox*>(scene->focusItem());
            text_box->setDefaultTextColor(textColor);
        }
    }
    selected_color = textColor;
}

void ToolBar::edit_done()
{
    emit view_visible(false);
    switch (current_tool) {
    case TEXTTOOL:{
        addText->setChecked(false);
        text_adjust->hide();
        text_adjust->deleteLater();
        current_tool = NONE;
        break;}
    default:
        break;
    }
}

void ToolBar::change_text_size(const QString &size)
{
    if (scene->focusItem() != nullptr){
        switch (scene->focusItem()->type()) {
        case TextBox::Type:{
            // auto item = static_cast<TextBox*>(scene->focusItem());
            // QFont font = item->font();
            // font.setPointSizeF(size.toDouble());
            // item->setFont(font);
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
        QFont temp_font = font;
        temp_font.setFamilies({font.family(), "Noto Color Emoji"});
        this->text_font->setCurrentFont(temp_font);
        auto item = static_cast<TextBox*>(scene->focusItem());
        item->setFont(temp_font);
        // qDebug() << "text font changed!";
    }
}

bool ToolBar::event(QEvent* event)
{
    const bool ret_val = QWidget::event(event);
    if (to_show_bar){
        paint_count++;
        if (event->type() == QEvent::WindowActivate){
            // QProcess process;
            QString pos;
            QString output;
            const auto gotSignal = [&output](QString response) {
                output = response;
            };
            QMetaObject::Connection conn = QObject::connect(toolbar_socket,
                                                            &HyprSocket::hypr_response, gotSignal);
            toolbar_socket->sendCommand("clients");
            // process.start("bash", QStringList() << "-c" << "hyprctl clients");
            // process.waitForFinished();
            // QString output = process.readAll();
            // 简单解析窗口 ID
            QObject::disconnect(conn);
            QStringList lines = output.split("\n");
            // QString window_seq = "1";
            for (int i = 0; i < lines.size(); ++i) {
                if (lines[i].contains(QString("title: ") + "capturepinTool." + window_title.split(".")[1])){
                    // qDebug() << "show";
                    for (int i = 0; i < lines.size(); ++i) {
                        if (lines[i].contains("title: " + window_title)) {
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
                    // process.start("bash", QStringList() << "-c" << "hyprctl dispatch movewindowpixel exact " + temp_cmd + "capturepinTool." + window_title.split(".")[1]);
                    // process.waitForFinished();
                    toolbar_socket->sendCommand("dispatch movewindowpixel exact " +
                                                temp_cmd + "capturepinTool." +
                                                window_title.split(".")[1]);
                    if (reshow_bar){
                        toolbar_socket->sendCommand("dispatch focuswindow title:" + window_title);
                        reshow_bar = false;
                    }
                    toolbar_socket->sendCommand(QString("dispatch setprop title:") + "capturepinTool." +
                                                window_title.split(".")[1] + " norounding 1");
                    to_show_bar = false;
                    paint_count = 0;
                }
            }

        }
    }
    return ret_val;
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
    if (!text_adjust.isNull() && newFocusItem != nullptr){
        switch (newFocusItem->type()) {
        case TextBox::Type:
        {
            auto item = static_cast<TextBox*>(newFocusItem);
            text_font->setCurrentFont(item->font());
            text_size->setCurrentText(QString::number(item->font().pointSizeF()));
            break;
        }
        default:
            break;
        }
    }
}

void ToolBar::get_selected_color(const QColor &color)
{
    if (scene->focusItem() != nullptr){
        switch (scene->focusItem()->type()) {
        case TextBox::Type:
        {
            auto text_box = static_cast<TextBox*>(scene->focusItem());
            text_box->setDefaultTextColor(color);
            break;
        }
        default:
            break;
        }
    }
    selected_color = color;
}

void ToolBar::screen_clicked(QMouseEvent *event)
{
    // qDebug() << "not_hovering";
    switch (current_tool) {
    case TEXTTOOL:{
        TextBox* text_box_1 = new TextBox();
        // connect(text_box_1, &TextBox::select_textbox, this, &GraphView::current_object);
        // text_box_1->setTextWidth(10);
        // text_box_1->setPlainText("可调整大小和旋转的文本框");
        // QFont font("Noto Sans Mono");
        // font.setStyleHint(QFont::Monospace);
        // font.setFamilies({"Noto Sans Mono", "Noto Color Emoji"});
        QFont font = text_font->currentFont();
        font.setPointSizeF(text_size->currentText().toDouble());
        text_box_1->setFont(font);
        if (selected_color.isValid()){
            text_box_1->setDefaultTextColor(selected_color);
        }
        text_box_1->setPos(event->scenePosition());
        qDebug() << event->scenePosition();
        qDebug() << text_box_1->scenePos();
        text_box_1->setScale(2);
        text_box_1->setTransformOriginPoint(text_box_1->boundingRect().center());
        // qDebug() << "add_text";
        scene->addItem(text_box_1);
        // qDebug() << text_box_1->isVisible();
        // connect(text_box_1, &TextBox::select_textbox, this, &ToolBar::get_selected_textbox);
        connect(text_box_1, &TextBox::update_font_size, this, &ToolBar::update_font_size);
        connect(text_box_1, &TextBox::get_font_size, this, &ToolBar::get_font_size);
        scene->update();
        break;}
    default:
        break;
    }
}

void ToolBar::update_font_size(double size)
{
    this->text_size->setCurrentText(QString::number(size));
}

double ToolBar::get_font_size()
{
    return this->text_size->currentText().toDouble();
}
