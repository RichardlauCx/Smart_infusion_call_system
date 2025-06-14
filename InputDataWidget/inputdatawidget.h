#ifndef INPUTDATAWIDGET_H
#define INPUTDATAWIDGET_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class InputDataWidget; }
QT_END_NAMESPACE

class InputDataWidget : public QMainWindow
{
    Q_OBJECT

public:
    InputDataWidget(QWidget *parent = nullptr);
    ~InputDataWidget();

    void write_file(QStringList data);
    QStringList read_file();
private slots:
    void on_addBt_clicked();
    void on_yesBt_clicked();
private:
    Ui::InputDataWidget *ui;
    QStringList dataList;
};
#endif // INPUTDATAWIDGET_H
