#include "clientwidget.h"
#include "ui_clientwidget.h"
#include <QDebug>
#include <QDate>
#include <QTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include <QMouseEvent>
#include <QInputDialog>
ClientWidget::ClientWidget(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientWidget)
{
    ui->setupUi(this);
    bednumber = 1;

    //计算总滴数
    allnumber = 50/0.01;
    //配置串口
    sport.setBaudRate(9600);//波特率
    sport.setStopBits(QSerialPort::OneStop);//停止位
    sport.setDataBits(QSerialPort::Data8);//设置数据位8位
    sport.setParity(QSerialPort::NoParity);//设置校验为无
    //打开设备
    sport.setPortName("COM1");//设置串口名称,linux系统"/dev/pts/x"
    if(sport.open(QIODevice::ReadWrite))//读写方式打开串口
    {
        qDebug()<<"打开成功";
    }else
    {
        qDebug()<<"打开失败";
        exit(0);
    }
    //当串口有数据就会发送readyRead信号
    connect(&sport, &QSerialPort::readyRead, this, &ClientWidget::read_data);

    //开启定时器
    updateid = startTimer(1000);


    //进程停止， 开始
    connect(&mprocess, SIGNAL(finished(int, QProcess::ExitStatus)),this, SLOT(mplayer_stop()));
    connect(&mprocess, SIGNAL(started()),this, SLOT(mplayer_start()));

    //网络初始化
    //连接成功的信号， 断开连接的信号
    connect(&msocket, &QTcpSocket::connected, this, &ClientWidget::connect_suc);
    connect(&msocket, &QTcpSocket::disconnected,this, &ClientWidget::connect_dis);
    cntimerid = startTimer(3000);
    //当客户端有数据到达的时候会发送readyRead信号
    connect(&msocket, &QTcpSocket::readyRead, this, &ClientWidget::read_net_data);
}

void ClientWidget::read_data()
{
    QString data = sport.readAll();//读取串口数据
    //解析数据 "[30:58]"
    //删除字符串中的[,]
    data.remove("[").remove("]");
    //把字符串按:分割成两个字符串的列表
    QStringList list = data.split(":");
    msec = list.at(0).toInt();
    number = list.at(1).toInt();
    sp = number/(msec/60.0);//计算每分钟的滴数
    ui->cDorvalue_2->setText(QString("%1d/min").arg((int)sp));
    //计算还需要滴多少时间
    double t = (allnumber - number)/sp;
    ui->cDorvalue_3->setText(QString("%1min").arg((int)t));

    //上传输液数据
    QString upstr =QString("{\"number\":%1,\"time\":%2,\"speed\":%3,\"flag\":0}").arg(bednumber)
            .arg((int)t).arg((int)sp);
    msocket.write(upstr.toUtf8());
}

ClientWidget::~ClientWidget()
{
    delete ui;
}

void ClientWidget::timerEvent(QTimerEvent *e)
{
    //判断是否是更新时间的定时器
    if(updateid == e->timerId())
    {
        QString dateStr = QDate::currentDate().toString("yyyy-MM-dd");
        QString timeStr = QTime::currentTime().toString("hh:mm:ss");
        ui->fTimeDate->setText(dateStr);
        ui->fTimeTime->setText(timeStr);
    }else  if(cntimerid == e->timerId())
    {
//        msocket.connectToHost("192.168.1.182",9999);
        msocket.connectToHost("192.168.1.104",9999);
    }
}

void ClientWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QPoint p = event->pos();
    if(ui->bNumber->geometry().contains(p))
    {
        int num = QInputDialog::getInt(this, "设置床位号", "请输入床位编号（1~12）");
        if (num == 0) return;
        bednumber = num;

        ui->bNumbervalue->setText(QString::number(bednumber));

        //，并且从服务器获取患者信息
        QString upstr = QString("{\"number\":%1,\"time\":60,\"speed\":110,\"flag\":2}").arg(bednumber);
        msocket.write(upstr.toUtf8());
    }
}


void ClientWidget::on_playerBt_clicked()
{
#ifdef WIN32

        //播放mp3--window
        mprocess.start("E:/software/Tools/mplayerpc/mplayer.exe  "
                       "E:/software/Tools/mplayerpc/555.mp3");  //需要修改mplayer路径和mp3路径
#else
        //播放mp3---linux
        mprocess.start("mplayer /mnt/hgfs/hunan/day5/mplayerpc/999.mp3");  //mp3路径需要自己修改
#endif

       //呼叫服务器
        //上传输液数据
        QString upstr =QString("{\"number\":%1,\"time\":0,\"speed\":0,\"flag\":1}").arg(bednumber);
        msocket.write(upstr.toUtf8());

}

void ClientWidget::mplayer_start()
{
    //拨放按钮失效
    ui->playerBt->setEnabled(false);
}

void ClientWidget::mplayer_stop()
{
    //拨放按钮生效
    ui->playerBt->setEnabled(true);
}

void ClientWidget::stopBt_clicked()
{
    mprocess.kill();//关闭进程
}

void ClientWidget::connect_suc()
{
    //连接服务器成功
    //上传床号，并且从服务器获取患者信息
    QString upstr = "{\"number\":1,\"time\":60,\"speed\":110,\"flag\":2}";
    msocket.write(upstr.toUtf8());
    qDebug()<<"连接服务器成功";
    //关闭定时器
    killTimer(cntimerid);
}

void ClientWidget::connect_dis()
{
    qDebug()<<"断开服务器";
    //启动定时器
    cntimerid = startTimer(3000);
}

void ClientWidget::read_net_data()
{
    //读取数据"{name:晴天,doctor:秦羽,nurse:秦瑶,capacity:20,flag:0}"
    QString data = msocket.readAll();
    qDebug()<<data;
#if 0
    //解析数据显示在界面上，---下面数据要通过解析data得到
    QString name = "秦天";
    QString doctor="秦羽";
    QString nurses="秦瑶";
    int capacity=20;
    int flag = 0;
#endif
    //解析数据--json数据
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8(),&err);
    if(err.error != QJsonParseError::NoError)
    {
        qDebug()<<"json格式错误";
        return ;
    }
    //创建json对象
    QJsonObject obj = doc.object();
    QString name = obj.value("name").toString();
    QString doctor = obj.value("doctor").toString();
    QString nurses = obj.value("nurses").toString();
    double capacity = obj.value("capacity").toDouble();
    int flag = obj.value("flag").toInt();
    //姓名，医师，护士，容量，标志0-不处理，1-呼叫得到呼应（停止呼叫铃声）
    ui->cDorvalue->setText(doctor);
    ui->aNamevalue->setText(name);
    ui->cDorrvalue->setText(nurses);
    allnumber = capacity/0.01;
    if(flag==1)
    {
        stopBt_clicked();
    }
}
