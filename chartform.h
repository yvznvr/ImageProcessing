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
    void setData(int*, int);
    void drawPlot();

private:
    Ui::ChartForm *ui;
    QCPBars *bars1;
    QVector<double> *dataOfHistogram;
    int max = 0;    // max data value to scale y axis of chart
};

#endif // CHARTFORM_H
