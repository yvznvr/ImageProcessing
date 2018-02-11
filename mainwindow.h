#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "bmpfile.h"

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

private:
    Ui::MainWindow *ui;
    BmpFile f;
};

#endif // MAINWINDOW_H
