#include "rectform.h"
#include "ui_rectform.h"
#include <QWidget>
#include <QString>

RectForm::RectForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RectForm)
{
    ui->setupUi(this);
}

RectForm::~RectForm()
{
    delete ui;
}

void RectForm::on_pushButton_clicked()
{
    QVector<int> vector;
    vector.append(ui->lineEdit_x1->text().toInt());
    vector.append(ui->lineEdit_y1->text().toInt());
    vector.append(ui->lineEdit_x2->text().toInt());
    vector.append(ui->lineEdit_y2->text().toInt());
    emit dataReady(vector);
}
