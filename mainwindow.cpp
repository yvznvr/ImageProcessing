#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDir>
#include <QString>
#include <QPixmap>
#include <QDebug>
#include <QPushButton>
#include <QEventLoop>


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
    QString outName = ui->outNameLine->text();
    if(outName.isEmpty()) outName = "output";
    f.ReadImage(ui->imageLine->text().toUtf8().constData());
    string path = (QDir::currentPath() + "/outputs/" + outName).toUtf8().constData();
    vect.clear();   // clear old data
    if(ui->comboBox->currentText() == "Convert Grayscale")
    {
        f.grayScale(path);
        QPixmap im(QDir::currentPath() + "/outputs/" + outName + ".bmp");
        ui->image2->setPixmap(im);
    }
    else if(ui->comboBox->currentText() == "Histogram")
    {
        f.grayScale(path);
        chartForm.setData(f.histogramData(),256);
        chartForm.drawPlot();
        chartForm.show();
    }
    if(ui->comboBox->currentText() == "Histogram Equalization")
    {
        f.grayScale(path);
        f.histogramEqualization();
        f.ExportImage(path);
        QPixmap im(QDir::currentPath() + "/outputs/" + outName + ".bmp");
        ui->image2->setPixmap(im);
    }
    else if(ui->comboBox->currentText() == "Draw Rect")
    {
        rectForm.show();
        QObject::connect(&rectForm,SIGNAL(dataReady(QVector<int>)),this,SLOT(getData(QVector<int>)));
        QPushButton *button = rectForm.findChild<QPushButton*>("pushButton");   // instance of button in toplevel window
        QEventLoop loop;
        QObject::connect(button, SIGNAL(clicked(bool)),&loop, SLOT(quit()));
        loop.exec();    // wait to clicked button
        rectForm.close();
        f.grayScale(path);
        f.drawRect(vect.at(0),vect.at(1),vect.at(2),vect.at(3));
        vect.clear();   // clear old data
        f.ExportImage(path);
        QPixmap im(QDir::currentPath() + "/outputs/" + outName + ".bmp");
        ui->image2->setPixmap(im);
    }
    else if(ui->comboBox->currentText() == "Draw Circle")
    {
        circleForm.show();
        QObject::connect(&circleForm,SIGNAL(dataReady(QVector<int>)),this,SLOT(getData(QVector<int>)));
        QPushButton *button = circleForm.findChild<QPushButton*>("pushButton");   // instance of button in toplevel window
        QEventLoop loop;
        QObject::connect(button, SIGNAL(clicked(bool)),&loop, SLOT(quit()));
        loop.exec();    // wait to clicked button
        circleForm.close();
        f.grayScale(path);
        f.drawCircle(vect.at(0),vect.at(1),vect.at(2));
        vect.clear();
        f.ExportImage(path);
        QPixmap im(QDir::currentPath() + "/outputs/" + outName + ".bmp");
        ui->image2->setPixmap(im);
    }
    else if(ui->comboBox->currentText() == "Draw Ellipse")
    {   ellipseForm.show();
        QObject::connect(&ellipseForm,SIGNAL(dataReady(QVector<int>)),this,SLOT(getData(QVector<int>)));
        QPushButton *button = ellipseForm.findChild<QPushButton*>("pushButton");   // instance of button in toplevel window
        QEventLoop loop;
        QObject::connect(button, SIGNAL(clicked(bool)),&loop, SLOT(quit()));
        loop.exec();    // wait to clicked button
        ellipseForm.close();
        f.grayScale(path);
        f.drawEllipse(vect.at(0),vect.at(1),vect.at(2),vect.at(3));
        vect.clear();
        f.ExportImage(path);
        QPixmap im(QDir::currentPath() + "/outputs/" + outName + ".bmp");
        ui->image2->setPixmap(im);
    }
    else if(ui->comboBox->currentText() == "Gaussian Mask")
    {
        float gaussian[9] = {0.25,0.5,0.25,0.5,1,0.5,0.25,0.5,0.25};
        f.maskApply(3,3, gaussian);
        QPixmap im(QDir::currentPath() + "/outputs/maske.bmp");
        ui->image2->setPixmap(im);
    }
    else if(ui->comboBox->currentText() == "Laplace Filter")
    {
        float laplace[9] = {0,1,0,1,-4,1,0,1,0};
        f.maskApply(3,3, laplace);
        QPixmap im(QDir::currentPath() + "/outputs/maske.bmp");
        ui->image2->setPixmap(im);
    }
    else if(ui->comboBox->currentText() == "Zoom")
    {
        rectForm.show();
        QObject::connect(&rectForm,SIGNAL(dataReady(QVector<int>)),this,SLOT(getData(QVector<int>)));
        QPushButton *button = rectForm.findChild<QPushButton*>("pushButton");   // instance of button in toplevel window
        QEventLoop loop;
        QObject::connect(button, SIGNAL(clicked(bool)),&loop, SLOT(quit()));
        loop.exec();    // wait to clicked button
        rectForm.close();
        f.grayScale(path);
        f.zoom(vect.at(0),vect.at(1),vect.at(2),vect.at(3));
        vect.clear();   // clear old data
        //f.ExportImage(path);
        QPixmap im(QDir::currentPath() + "/outputs/zoom.bmp");
        ui->image1->setPixmap(im);
        ui->imageLine->setText(QDir::currentPath() + "/outputs/zoom.bmp");
    }
    else if(ui->comboBox->currentText() == "KMeans")
    {
        f.grayScale(path);
        f.kmeans();
        QPixmap im(QDir::currentPath() + "/outputs/kmeans.bmp");
        ui->image2->setPixmap(im);
    }
    else if(ui->comboBox->currentText() == "Multi Dimensional KMeans")
    {
        f.coloredKmeans();
        QPixmap im(QDir::currentPath() + "/outputs/kmeans2.bmp");
        ui->image2->setPixmap(im);
    }
}

void MainWindow::getData(QVector<int> vector)
{
    for(int i=0; i<vector.size();i++)
        vect.append(vector.at(i));
}
