#include "settings_window.h"
#include "ui_settings_window.h"

using namespace presentation;

SettingsWindow::SettingsWindow(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    QWidget::setWindowFlags(Qt::Window);
    setFixedSize(this->width(), this->height());
    connect(ui->okButton, &QPushButton::clicked, this, &SettingsWindow::close);
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}
