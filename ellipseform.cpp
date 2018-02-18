#include "ellipseform.h"
#include "ui_ellipseform.h"

EllipseForm::EllipseForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EllipseForm)
{
    ui->setupUi(this);
}

EllipseForm::~EllipseForm()
{
    delete ui;
}

void EllipseForm::on_pushButton_clicked()
{
    QVector<int> vector;
    vector.append(ui->lineEdit_x1->text().toInt());
    vector.append(ui->lineEdit_y1->text().toInt());
    vector.append(ui->lineEdit_x2->text().toInt());
    vector.append(ui->lineEdit_y2->text().toInt());
    emit dataReady(vector);
}
