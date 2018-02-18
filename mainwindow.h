#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "bmpfile.h"
#include "rectform.h"
#include "circleform.h"
#include "ellipseform.h"
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
private:
    Ui::MainWindow *ui;
    BmpFile f;
    RectForm rectForm;
    CircleForm circleForm;
    EllipseForm ellipseForm;
    QVector<int> vect;
};

#endif // MAINWINDOW_H
