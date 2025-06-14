#ifndef DATESENSOR_H
#define DATESENSOR_H

#include <QMainWindow>
#include <QSerialPort>
QT_BEGIN_NAMESPACE
namespace Ui { class DateSensor; }
QT_END_NAMESPACE
class DateSensor : public QMainWindow
{
    Q_OBJECT
public:
    DateSensor(QWidget *parent = nullptr);
    ~DateSensor();
//定时器事件
    void timerEvent(QTimerEvent* e);
private slots:
    void on_resetBt_clicked();
    void on_startBt_clicked();
    void on_spinBox_valueChanged(int arg1);

private:
    Ui::DateSensor *ui;
    QSerialPort sport;//串口设备对象
    int timerid;//定时器事件id
    int number;//保存滴数
    int msec;//记录秒
    int pre;
};
#endif // DATESENSOR_H
