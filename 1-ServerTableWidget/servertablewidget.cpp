#include "servertablewidget.h"
#include "ui_servertablewidget.h"
#include "form.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include <QDate>
#include <QFile>

ServerTableWidget::ServerTableWidget(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ServerTableWidget)
{
    ui->setupUi(this);
    //启动更新时间的定时器事件
    updatetmId = startTimer(1000);
    //启动更新患者信息
    updateinfoId = startTimer(3000);

    //关联客户端连接信号
    connect(&mserver, &QTcpServer::newConnection, this, &ServerTableWidget::new_client);
    //启动服务器
    mserver.listen(QHostAddress::Any, 9999);



    //插入行和列-项目
    for(int i=0;i<3;i++)
        ui->tableWidget->insertRow(i);
    for(int i=0;i<4;i++)
        ui->tableWidget->insertColumn(i);
    for(int r=0; r<3; ++r)
    {

        for(int c=0; c<4; ++c)
        {
            Form *form = new Form();
            form->setNumber(QString::number(r*4+c+1));
            ui->tableWidget->setCellWidget(r, c, form);
        }
    }

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}




ServerTableWidget::~ServerTableWidget()
{
    delete ui;
}


QStringList ServerTableWidget::read_file()
{
    //创建QFile对象
    QFile file("F:/Computer/UI/QT/Project/project_1_2/file.txt"); //注意路径不要错了
    //打开文件
    bool flag = file.open(QIODevice::ReadOnly); //只读
    if(!flag)
    {
        qDebug()<<"打开文件失败";
        exit(1);
    }
    QStringList datalist;
    while(!file.atEnd())
    {
        QString data = file.readLine(); //读取一行数据
        datalist<<data;//把读到的数据添加到列表中
    }
    //关闭文件
    file.close();
    return datalist;
}


void ServerTableWidget::timerEvent(QTimerEvent *e)
{
    QString datestr = QDate::currentDate().toString("yyyy-MM-dd");
    ui->dateLb->setText(datestr);
    QString timestr = QTime::currentTime().toString("hh:mm:ss");
    ui->timeLb->setText(timestr);

    if(updateinfoId == e->timerId())
    {
        //从文件中提取床号患者信息  //床号:姓名:医师:护士:容量
        QStringList infolist = read_file();
        //设置患者床位信息
        for(int i=0; i<infolist.count();i++)
        {
            QString tmp = infolist.at(i);
            QStringList tmplist = tmp.split(":");
            int number=tmplist.at(0).toInt()-1;
            QString name=tmplist.at(1);
            QString doctor=tmplist.at(2);
            QString nurses=tmplist.at(3);
            int cap = tmplist.at(4).toInt();
            int r=number/4;
            int c=number-r*4;
            Form *form = (Form*)ui->tableWidget->cellWidget(r, c);
            form->setInfo(name, doctor, nurses, cap);
        }
    }
}

void ServerTableWidget::new_client()
{
    qDebug()<<"新客户端连接";
    //获取与客户端通信的套接字
    QTcpSocket* msocket = mserver.nextPendingConnection();
    //关联读数据信号， 掉线信号
    connect(msocket, &QTcpSocket::readyRead, this, &ServerTableWidget::data_client);
    connect(msocket,&QTcpSocket::disconnected, this, &ServerTableWidget::dis_client);

    //保存套接字
    sockets.append(msocket);
    //把这个套接字的ip地址显示在界面上
    //ui->clientlist->addItem(msocket->peerAddress().toString().remove("::ffff:"));
}

void ServerTableWidget::dis_client()
{
    //获取客户端套接字
    QTcpSocket *msocket = (QTcpSocket*)sender();
    //在容器中查找掉线的客户端
    int row = sockets.indexOf(msocket);
    //删除界面的上ip
    //delete  ui->clientlist->item(row);
    //删除掉线的套接字
    sockets.removeOne(msocket);
}

void ServerTableWidget::data_client()
{
    //获取客户端套接字
    QTcpSocket *msocket = (QTcpSocket*)sender();
    QString data = msocket->readAll();
    qDebug()<<msocket->peerAddress().toString().remove("::ffff:")+":"+data;
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
    int number = obj.value("number").toInt();
    double time = obj.value("time").toDouble();
    double speed = obj.value("speed").toDouble();
    int flag = obj.value("flag").toInt();

    qDebug()<<number<<time<<speed<<flag;
    number-=1;
    //根据number获取患者信息
    int row = number/4;
    int col = number-(row*4);
    qDebug()<<row<<col;
    Form *form = (Form*) ui->tableWidget->cellWidget(row,col);


    if(flag == 0)
    {
        form->setSp(QString("%1d/min").arg(speed));
        form->setTm(QString("%1min").arg(time));
    }else if(flag==2)//当客户端i第一次连接服务器时候初始化患者信息到呼叫终端
    {
        InfoData inf = form->getInfo();
        //打包数据发送给客户端
        QString reply=QString("{\"name\":\"%1\",\"doctor\":\"%2\",\"nurses\":\"%3\",\"capacity\":%4,\"flag\":0}")
                .arg(inf.name).arg(inf.doctor).arg(inf.nurses).arg(inf.size);
        msocket->write(reply.toUtf8());

        //保存套接字
        form->setMsocket(msocket);

    }else if(flag==1)//紧急呼叫
    {
        form->setStyleSheet("QWidget#Form{ \
                           border:1px solid #ff0000;\
                            border-radius:10px;\
                            background-color: rgba(255, 0, 0);}");
    }
}


void ServerTableWidget::on_tableWidget_cellDoubleClicked(int row, int column)
{
    //双击格子恢复到原来的颜色
    //获取窗口
    Form *form = (Form*)ui->tableWidget->cellWidget(row, column);
    form->setStyleSheet("QWidget{\
                        border:1px solid #ff0000;\
                        border-radius:5px;\
                        \
                        background-color: rgba(153, 155, 255,0.5);\
                    }\
                    QWidget#Form{\
                        border:1px solid #ff0000;\
                        border-radius:10px;\
                        background-color: rgba(206, 255, 247);\
                    }\
                    QLabel{\
                        border:none;\
                        background-color: rgba(153, 155, 255,0);\
                    }\
                    \
                    QWidget#w1{\
                    \
                        border:1px solid #ff0000;\
                        border-radius:5px;\
                        \
                        background-color: rgba(153, 155, 166,0.5);\
                    }");
    InfoData inf = form->getInfo();
    //打包数据发送给客户端
    QString reply=QString("{\"name\":\"%1\",\"doctor\":\"%2\",\"nurses\":\"%3\",\"capacity\":%4,\"flag\":1}")
            .arg(inf.name).arg(inf.doctor).arg(inf.nurses).arg(inf.size);
    QTcpSocket *msocket = form->getMsocket();qDebug()<<reply;
    if(msocket)
    {
        msocket->write(reply.toUtf8());
        qDebug()<<reply;
    }
}
