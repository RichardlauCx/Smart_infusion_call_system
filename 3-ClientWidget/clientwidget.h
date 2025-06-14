#ifndef CLIENTWIDGET_H
#define CLIENTWIDGET_H

#include <QMainWindow>
#include <QSerialPort>
#include <QTimerEvent>
#include <QProcess>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class ClientWidget; }
QT_END_NAMESPACE

class ClientWidget : public QMainWindow
{
    Q_OBJECT
public:
    ClientWidget(QWidget *parent = nullptr);
    ~ClientWidget();
//定时器事件
    void timerEvent(QTimerEvent *e);
public:
    virtual void mouseDoubleClickEvent(QMouseEvent *event);

protected slots:
    void read_data();//读取串口数据的槽函数
    void on_playerBt_clicked(); //点击播放
    void mplayer_start(); //开始后所做的事情
    void mplayer_stop(); //停止所需要做的事情
    void stopBt_clicked();

    //网络
    void connect_suc();
    void connect_dis();
    void read_net_data();

private:
    Ui::ClientWidget *ui;
    QSerialPort sport; //串口
    int msec; //接收输液时间
    int number; //接收输液的滴数
    //固定每瓶50ml，每一滴0.01ml
    double sp;//保存速度
    double allnumber; //总的滴数
    QProcess mprocess; //进程
    int updateid;

    //网络
    QTcpSocket msocket;
    int cntimerid;

    int bednumber;
};
#endif // CLIENTWIDGET_H
