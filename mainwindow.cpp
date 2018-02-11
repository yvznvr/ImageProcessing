#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDir>
#include <QString>
#include <QPixmap>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_imageButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Select Image","","Bmp Files (*.bmp)");
    ui->imageLine->setText(fileName);
    QPixmap im(fileName);
    ui->image1->setPixmap(im);
}

void MainWindow::on_applyButton_clicked()
{
    if(ui->comboBox->currentText() == "Convert Grayscale")
    {
        QString outName = ui->outNameLine->text();
        if(outName.isEmpty()) outName = "output";
        f.ReadImage(ui->imageLine->text().toUtf8().constData());
        string path = (QDir::currentPath() + "/outputs/" + outName).toUtf8().constData();
        f.grayScale(path);
        //f.ExportImage(path);
        QPixmap im(QDir::currentPath() + "/outputs/" + outName + ".bmp");
        ui->image2->setPixmap(im);

    }

}
