#ifndef CIRCLEFORM_H
#define CIRCLEFORM_H

#include <QWidget>
#include <QVector>
namespace Ui {
class CircleForm;
}

class CircleForm : public QWidget
{
    Q_OBJECT

public:
    explicit CircleForm(QWidget *parent = 0);
    ~CircleForm();

signals:
    void dataReady(QVector<int>);

private slots:
    void on_pushButton_clicked();

private:
    Ui::CircleForm *ui;
};

#endif // CIRCLEFORM_H
