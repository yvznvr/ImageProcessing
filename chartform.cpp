#include "chartform.h"


ChartForm::ChartForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChartForm)
{
    ui->setupUi(this);
    bars1 = new QCPBars(ui->customPlot->xAxis, ui->customPlot->yAxis);
}

ChartForm::~ChartForm()
{
    delete ui;
    delete dataOfHistogram;
    ui->customPlot->clearPlottables();
}

void ChartForm::setData(int *data, int size)
{
    dataOfHistogram = new QVector<double>;
    for(int i=0;i<size;i++)
    {
        dataOfHistogram->append(data[i]);
        if(data[i]>max) max=data[i];
    }
    max+=100;
}

void ChartForm::drawPlot()
{
    // generate some data:
    QVector<double> x(256);
    for (int i=0; i<256; ++i)
    {
      x[i] = i;
    }
    // create graph and assign data to it:
    if(ui->customPlot->plottableCount())
    {
        ui->customPlot->removePlottable(0);
        bars1 = new QCPBars(ui->customPlot->xAxis, ui->customPlot->yAxis);
    }
    //ui->customPlot->hasPlottable(bars1);
    bars1->addData(x, *dataOfHistogram);
    // give the axes some labels:
    //ui->customPlot->xAxis->setLabel("x");
    //ui->customPlot->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    ui->customPlot->xAxis->setRange(0, 256);
    ui->customPlot->yAxis->setRange(0, max);
    ui->customPlot->replot();
    max = 0;
}
