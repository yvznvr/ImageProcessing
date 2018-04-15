#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "bmpfile.h"
#include "rectform.h"
#include "circleform.h"
#include "ellipseform.h"
#include "chartform.h"
#include <QString>
#include <QVector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_imageButton_clicked();
    void on_applyButton_clicked();
    void getData(QVector<int>);
    void on_leftButton_clicked();

private:
    Ui::MainWindow *ui;
    BmpFile f;
    RectForm rectForm;
    CircleForm circleForm;
    EllipseForm ellipseForm;
    ChartForm chartForm;
    QVector<int> vect;
    QString exportPath = "";
};

#endif // MAINWINDOW_H
