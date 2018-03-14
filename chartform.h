#ifndef CHARTFORM_H
#define CHARTFORM_H

#include "ui_chartform.h"
#include <QWidget>
#include <QVector>


typedef unsigned char BYTE;

namespace Ui {
class ChartForm;
}

class ChartForm : public QWidget
{
    Q_OBJECT

public:
    explicit ChartForm(QWidget *parent = 0);
    ~ChartForm();
    void setData(BYTE*, int);
    void drawPlot();

private:
    Ui::ChartForm *ui;
    QCPBars *bars1;
    QVector<double> *dataOfHistogram;
};

#endif // CHARTFORM_H
