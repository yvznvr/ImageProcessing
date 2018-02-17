#ifndef RECTFORM_H
#define RECTFORM_H

#include <QWidget>
#include <QVector>

namespace Ui {
class RectForm;
}

class RectForm : public QWidget
{
    Q_OBJECT

public:
    explicit RectForm(QWidget *parent = 0);
    ~RectForm();
    int x1,y1,x2,y2;

private slots:
    void on_pushButton_clicked();

signals:
    void dataReady(QVector<int>);

private:
    Ui::RectForm *ui;
};

#endif // RECTFORM_H
