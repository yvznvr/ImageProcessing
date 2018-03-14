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
    delete bars1;
}

void ChartForm::setData(BYTE *data, int size)
{
    dataOfHistogram = new QVector<double>;
    for(int i=0;i<size;i++)
    {
        dataOfHistogram->append(data[i]);
    }
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
    bars1->data().clear();
    ui->customPlot->hasPlottable(bars1);
    bars1->addData(x, *dataOfHistogram);
    // give the axes some labels:
    //ui->customPlot->xAxis->setLabel("x");
    //ui->customPlot->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    ui->customPlot->xAxis->setRange(0, 256);
    ui->customPlot->yAxis->setRange(0, 320);
    ui->customPlot->replot();
}
