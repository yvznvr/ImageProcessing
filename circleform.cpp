#include "circleform.h"
#include "ui_circleform.h"

CircleForm::CircleForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CircleForm)
{
    ui->setupUi(this);
}

CircleForm::~CircleForm()
{
    delete ui;
}

void CircleForm::on_pushButton_clicked()
{
    QVector<int> vector;
    vector.append(ui->lineEdit_x1->text().toInt());
    vector.append(ui->lineEdit_y1->text().toInt());
    vector.append(ui->lineEdit_x2->text().toInt());
    emit dataReady(vector);
}
