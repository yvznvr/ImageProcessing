#ifndef ELLIPSEFORM_H
#define ELLIPSEFORM_H

#include <QWidget>
#include <QVector>

namespace Ui {
class EllipseForm;
}

class EllipseForm : public QWidget
{
    Q_OBJECT

public:
    explicit EllipseForm(QWidget *parent = 0);
    ~EllipseForm();

private slots:
    void on_pushButton_clicked();

signals:
    void dataReady(QVector<int>);
private:
    Ui::EllipseForm *ui;
};

#endif // ELLIPSEFORM_H
