#include "about_dialog.h"
#include "ui_about_dialog.h"

#include <QImage>
#include <QDebug>

using namespace presentation;

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    QWidget::setWindowFlags(Qt::Dialog);
    setFixedSize(540,320);
    connect(ui->closeButton, &QPushButton::clicked, this, &AboutDialog::close);
    QImage img1;
    QImage img2;
    img1.load(":/images/andr.jpg");
    img2.load(":/images/ger.jpg");
    ui->pict1->setPixmap(QPixmap::fromImage(img1));
    ui->pict2->setPixmap(QPixmap::fromImage(img2));
    ui->titleLabel->setText("LIDAR Peripheral Control ");
}

AboutDialog::~AboutDialog()
{
    qDebug()<<"aboutDialog destroyed";
    delete ui;
}
