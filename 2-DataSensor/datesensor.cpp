#include "datesensor.h"
#include "ui_datesensor.h"
#include <QDebug>
DateSensor::DateSensor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DateSensor)
{
    ui->setupUi(this);
    pre = 1;
}

DateSensor::~DateSensor()
{
    delete ui;
}

void DateSensor::timerEvent(QTimerEvent *e)
{
    //发送数据到串口
    //打包数据
    int t = time(NULL);//现在的时间
    QString  data=QString("[%1:%2]").arg(t-msec).arg(number);
    ui->timeLb->setText(QString::number(t-msec)); //在界面显示
    number += pre;//每一秒钟pre滴
    ui->numberLb->setText(QString::number(number));
    qDebug()<<data;
    sport.write(data.toUtf8());
}

void DateSensor::on_resetBt_clicked()
{
    //停止定时器
    killTimer(timerid);
    //关闭串口
    sport.close();
    //复位时间和数据
    msec = 0;
    number=0;
}

void DateSensor::on_startBt_clicked()
{
    //配置串口
    sport.setBaudRate(9600);//波特率
    sport.setStopBits(QSerialPort::OneStop);//停止位
    sport.setDataBits(QSerialPort::Data8);//设置数据位8位
    sport.setParity(QSerialPort::NoParity);//设置校验为无
    //打开设备
    sport.setPortName("COM2");//设置串口名称,linux系统"/dev/pts/x"
    if(sport.open(QIODevice::ReadWrite))//读写方式打开串口
    {
        qDebug()<<"打开成功";
    }else
    {
        qDebug()<<"打开失败";
        return;
    }

    //启动定时器事件
    timerid = startTimer(1000);
    //记录开始的时间
    number=0;
    msec = time(NULL);
}

void DateSensor::on_spinBox_valueChanged(int arg1)
{
    pre = arg1;
}
